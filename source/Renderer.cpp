#include "AssetManager.h"
#include "CameraComponent.h"
#include "Constants.h"
#include "Context.h"
#include "DebugDrawer.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GLIncludes.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
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

struct Viewport {
   int x;
   int y;
   int width;
   int height;

   Viewport(int x, int y, int width, int height)
      : x(x), y(y), width(width), height(height) {
   }
};

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

Viewport getViewport(int camera, int numCameras, int framebufferWidth, int framebufferHeight) {
   ASSERT(numCameras > 0 && numCameras <= MAX_PLAYERS, "Number of cameras is invalid: %d", numCameras);
   ASSERT(camera >= 0 && camera < numCameras, "Invalid camera number: %d (%d total)", camera, numCameras);

   Viewport viewport(0, 0, framebufferWidth, framebufferHeight);

   if (numCameras == 1) {
      return viewport;
   }

   int halfWidth = framebufferWidth / 2 + framebufferWidth % 2;
   int halfHeight = framebufferHeight / 2 + framebufferHeight % 2;

   viewport.width = halfWidth;
   viewport.height = halfHeight;

   if (numCameras == 2) {
      viewport.x = halfWidth / 2;
      viewport.y = camera == 0 ? halfHeight : 0;
   } else {
      viewport.x = camera % 2 == 0 ? 0 : halfWidth;
      viewport.y = camera > 1 ? 0 : halfHeight;
   }

   return viewport;
}

} // namespace

Renderer::Renderer()
   : clearFramesNeeded(0), renderDebug(false) {
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

   // Transparency
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   this->fov = fov;

   onFramebufferSizeChange(width, height);

   hudRenderer.init();

   postProcessRenderer.init();

   debugRenderer.init();
}

void Renderer::onFramebufferSizeChange(int width, int height) {
   this->width = width;
   this->height = height;
   clearFramesNeeded = 2; // Clear both the front and back buffers

   projectionMatrix = glm::perspective(glm::radians(fov), (float)width / height, 0.1f, 200.0f);
}

void Renderer::render(Scene &scene) {
   RUN_DEBUG(checkGLError();)

   GLbitfield mask = GL_DEPTH_BUFFER_BIT;
   if (clearFramesNeeded) {
      --clearFramesNeeded;
      mask |= GL_COLOR_BUFFER_BIT;
   }
   glClear(mask);

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
      Viewport viewport(getViewport(i, numCameras, width, height));
      glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

      renderFromCamera(scene, *cameras[i]);
   }
}

void Renderer::renderFromCamera(Scene &scene, const GameObject &camera) {
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

   glDisable(GL_DEPTH_TEST);

   PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&camera.getLogicComponent());

   if (playerLogic) {
      hudRenderer.render(*playerLogic, width, height);

      int playerNum = camera.getInputComponent().getPlayerNum();
      if (scene.getGameState().hasWinner() && scene.getGameState().getWinner() == playerNum) {
         float runningTime = Context::getInstance().getRunningTime() * 5.0f;
         float offset = glm::pi<float>() * 2.0f / 3.0f;
         float red = (glm::sin(runningTime) + 1.0f) / 2.0f;
         float green = (glm::sin(runningTime + offset) + 1.0f) / 2.0f;
         float blue = (glm::sin(runningTime + offset * 2.0f) + 1.0f) / 2.0f;
         postProcessRenderer.render(0.5f, glm::vec3(red, green, blue));
      }

      if (!playerLogic->isAlive()) {
         float opacity = 0.75f * glm::min(1.0f, playerLogic->timeSinceDeath());
         postProcessRenderer.render(opacity, glm::vec3(0.0f));
      }
   }

   glEnable(GL_DEPTH_TEST);
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
