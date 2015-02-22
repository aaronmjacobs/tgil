#include "AssetManager.h"
#include "CameraComponent.h"
#include "Constants.h"
#include "Context.h"
#include "DebugDrawer.h"
#include "FancyAssert.h"
#include "Framebuffer.h"
#include "GameObject.h"
#include "GLIncludes.h"
#include "GraphicsComponent.h"
#include "LightComponent.h"
#include "LogHelper.h"
#include "Model.h"
#include "PhysicsManager.h"
#include "PlayerLogicComponent.h"
#include "Renderer.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "TextureMaterial.h"
#include "TextureUnitManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <set>
#include <string>
#include <vector>

namespace {

std::string getErrorName(GLenum error) {
   switch (error) {
      case GL_NO_ERROR:
         return "GL_NO_ERROR";
      case GL_INVALID_ENUM:
         return "GL_INVALID_ENUM";
      case GL_INVALID_VALUE:
         return "GL_INVALID_VALUE";
      case GL_INVALID_OPERATION:
         return "GL_INVALID_OPERATION";
      case GL_INVALID_FRAMEBUFFER_OPERATION:
         return "GL_INVALID_FRAMEBUFFER_OPERATION";
      case GL_OUT_OF_MEMORY:
         return "GL_OUT_OF_MEMORY";
      case GL_STACK_UNDERFLOW:
         return "GL_STACK_UNDERFLOW";
      case GL_STACK_OVERFLOW:
         return "GL_STACK_OVERFLOW";
      case GL_TABLE_TOO_LARGE:
         return "GL_TABLE_TOO_LARGE";
      default:
         return "Unknown";
   }
}

void checkGLError() {
   GLenum error = glGetError();
   if (error != GL_NO_ERROR) {
      LOG_WARNING("OpenGL error " << error << " (" << getErrorName(error) << ")");
   }
}

const glm::vec3 DEFAULT_CLEAR_COLOR(0.5f);

glm::vec3 getClearColor(const Scene &scene) {
   const GameState &gameState = scene.getGameState();
   if (!gameState.hasWinner()) {
      return DEFAULT_CLEAR_COLOR;
   }

   SPtr<GameObject> player = scene.getPlayerByNumber(gameState.getWinner());
   if (!player) {
      return DEFAULT_CLEAR_COLOR;
   }

   PlayerLogicComponent *playerLogic = static_cast<PlayerLogicComponent*>(&player->getLogicComponent());
   if (!playerLogic) {
      return DEFAULT_CLEAR_COLOR;
   }

   return playerLogic->getColor();
}

} // namespace

Renderer::Renderer()
   : renderDebug(false), framebuffer(new Framebuffer) {
}

Renderer::~Renderer() {
}

void Renderer::init(float fov, int width, int height) {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   // Depth Buffer Setup
   glClearDepth(1.0f);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   // Back face culling
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);

   this->fov = fov;

   loadPlane();

   onWindowSizeChange(width, height);

   debugRenderer.init();
}

void Renderer::loadPlane() {
   AssetManager &assetManager = Context::getInstance().getAssetManager();

   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/framebuffer");

   shaderProgram->addUniform("uColor");
   shaderProgram->addUniform("uDepth");
   shaderProgram->addUniform("uBrightness");
   shaderProgram->addUniform("uModelMatrix");
   shaderProgram->addAttribute("aPosition");
   shaderProgram->addAttribute("aTexCoord");

   colorTextureMaterial = std::make_shared<TextureMaterial>(*shaderProgram, 0, "uColor");
   depthTextureMaterial = std::make_shared<TextureMaterial>(*shaderProgram, 0, "uDepth");

   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xy_plane.obj");

   xyPlane = UPtr<Model>(new Model(shaderProgram, colorTextureMaterial, planeMesh));
   xyPlane->attachMaterial(depthTextureMaterial);
}

void Renderer::initFramebuffer() {
   framebuffer->init();

   ASSERT(colorTextureMaterial && depthTextureMaterial, "Trying to init framebuffer texture materials before they are created");
   colorTextureMaterial->setTextureID(framebuffer->getTextureID());
   depthTextureMaterial->setTextureID(framebuffer->getDepthTextureID());
}

void Renderer::onWindowSizeChange(int width, int height) {
   projectionMatrix = glm::perspective(glm::radians(fov), (float)width / height, 0.1f, 200.0f);
   initFramebuffer();
}

void Renderer::onMonitorChange() {
   // Update the framebuffer to match the size of the viewport
   initFramebuffer();
}

void Renderer::render(Scene &scene) {
   RUN_DEBUG(checkGLError();)

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Free all texture units
   Context::getInstance().getTextureUnitManager().reset();

   const std::vector<SPtr<GameObject>> &cameras = scene.getCameras();
   if (cameras.empty()) {
      LOG_WARNING("Scene must have camera to render");
   }

   int numCameras = cameras.size();
   if (numCameras > MAX_PLAYERS) {
      LOG_WARNING("More cameras than allowed number of players");
      numCameras = MAX_PLAYERS;
   }

   for (int i = 0; i < numCameras; ++i) {
      framebuffer->use();

      renderFromCamera(scene, *cameras[i]);

      framebuffer->disable();

      PlayerLogicComponent* logic = static_cast<PlayerLogicComponent*>(&cameras[i]->getLogicComponent());
      float brightness = logic && !logic->isAlive() ? 0.4f : 1.0f;
      renderFramebufferToPlane(i, cameras.size(), brightness);
   }
}

void Renderer::renderFromCamera(Scene &scene, const GameObject &camera) {
   glm::vec3 clearColor = getClearColor(scene);
   glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Set up shaders
   const CameraComponent &cameraComponent = camera.getCameraComponent();
   const glm::mat4 &viewMatrix = cameraComponent.getViewMatrix();
   const glm::vec3 &cameraPosition = cameraComponent.getCameraPosition();
   const std::vector<SPtr<GameObject>> &lights = scene.getLights();
   const std::set<SPtr<ShaderProgram>> &shaderPrograms = scene.getShaderPrograms();
   for (SPtr<ShaderProgram> shaderProgram : shaderPrograms) {
      shaderProgram->use();

      // Projection matrix
      GLint uProjMatrix = shaderProgram->getUniform("uProjMatrix");
      glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

      // View matrix
      GLint uViewMatrix = shaderProgram->getUniform("uViewMatrix");
      glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

      // Camera position
      if (shaderProgram->hasUniform("uCameraPos")) {
         GLint uCameraPos = shaderProgram->getUniform("uCameraPos");
         glUniform3fv(uCameraPos, 1, glm::value_ptr(cameraPosition));
      }

      // Lights
      if (shaderProgram->hasUniform("uNumLights")) {
         glUniform1i(shaderProgram->getUniform("uNumLights"), lights.size());
         unsigned int lightIndex = 0;
         for (SPtr<GameObject> light : lights) {
            light->getLightComponent().draw(*shaderProgram, lightIndex++);
         }
      }
   }

   // Skybox
   SPtr<GameObject> skybox = scene.getSkybox();
   if (skybox) {
      skybox->getGraphicsComponent().draw();
   }

   // Objects
   const std::vector<SPtr<GameObject>> &gameObjects = scene.getObjects();
   for (SPtr<GameObject> gameObject : gameObjects) {
      // Don't render the object that the camera is attached to
      if (&camera == gameObject.get()) {
         continue;
      }

      gameObject->getGraphicsComponent().draw();
   }

   if (renderDebug) {
      renderDebugInfo(scene, viewMatrix);
   }
}

void Renderer::renderDebugInfo(Scene &scene, const glm::mat4 &viewMatrix) {
   DebugDrawer &debugDrawer = scene.getDebugDrawer();

   // Instruct Bullet to generate debug drawing data
   scene.getPhysicsManager()->debugDraw();

   // Render the data
   debugRenderer.render(debugDrawer, viewMatrix, projectionMatrix);

   // Clear the data
   debugDrawer.clear();
}

void Renderer::renderFramebufferToPlane(int camera, int numCameras, float brightness) {
   ASSERT(xyPlane, "Plane not loaded");
   ASSERT(camera >= 0 && camera < numCameras, "Invalid camera number: %d", camera);
   ASSERT(numCameras > 0 && numCameras <= MAX_PLAYERS, "Invalid number of cameras: %d", numCameras);

   // Set offset and scale based off of camera number / camera count (for split-screen)
   glm::vec3 offset(0.0f);
   glm::vec3 scale(1.0f);
   if (numCameras > 1) {
      scale = glm::vec3(0.5f);

      if (numCameras > 2) {
         offset.x = camera % 2 == 0 ? -0.5f : 0.5f;
         offset.y = camera > 1 ? -0.5f : 0.5f;
      } else {
         offset.y = camera % 2 == 0 ? 0.5f : -0.5f;
      }
   }

   const glm::mat4 &transMatrix = glm::translate(offset);
   const glm::mat4 &scaleMatrix = glm::scale(scale);
   const glm::mat4 &modelMatrix = transMatrix * scaleMatrix;

   xyPlane->getShaderProgram()->use();
   glUniform1f(xyPlane->getShaderProgram()->getUniform("uBrightness"), brightness);
   glUniformMatrix4fv(xyPlane->getShaderProgram()->getUniform("uModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

   xyPlane->draw();
}
