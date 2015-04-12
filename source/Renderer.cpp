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
#include "RenderData.h"
#include "Renderer.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "ShadowMap.h"
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

   shadowMapManager = std::move(UPtr<ShadowMapManager>(new ShadowMapManager(ShadowMap::MAX_SHADOWS, ShadowMap::MAX_CUBE_SHADOWS)));

   onFramebufferSizeChange(width, height);

   hudRenderer.init();

   postProcessRenderer.init();

   skyRenderer.init();

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

   renderShadowMaps(scene);

   prepareLights(scene);

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

      renderFromCamera(scene, *cameras[i], viewport);
   }
}

void Renderer::renderShadowMaps(Scene &scene) {
   glDisable(GL_CULL_FACE);

   const std::vector<SPtr<GameObject>> &lights = scene.getLights();
   for (SPtr<GameObject> light : lights) {
      renderShadowMap(scene, light);
   }

   glEnable(GL_CULL_FACE);
}

void Renderer::prepareLights(Scene &scene) {
   const std::set<SPtr<ShaderProgram>> &shaderPrograms = scene.getShaderPrograms();
   const std::vector<SPtr<GameObject>> &lights = scene.getLights();
   GLenum shadowTextureUnit = Context::getInstance().getTextureUnitManager().getReservedShadowUnit();
   GLenum cubeShadowTextureUnit = Context::getInstance().getTextureUnitManager().getReservedCubeShadowUnit();

   for (SPtr<ShaderProgram> shaderProgram : shaderPrograms) {
      // Reset shadows to default textures
      if (shaderProgram->hasUniform("uShadows[0].shadowMap")) {
         for (int i = 0; i < ShadowMap::MAX_SHADOWS; ++i) {
            std::stringstream ss;
            ss << "uShadows[" << i << "].shadowMap";
            shaderProgram->setUniformValue(ss.str(), shadowTextureUnit);
         }
         for (int i = 0; i < ShadowMap::MAX_CUBE_SHADOWS; ++i) {
            std::stringstream ss;
            ss << "uCubeShadows[" << i << "].shadowMap";
            shaderProgram->setUniformValue(ss.str(), cubeShadowTextureUnit);
         }
      }

      // Render light / shadow info
      if (shaderProgram->hasUniform("uNumLights")) {
         int numLights = glm::min((int)lights.size(), LightComponent::MAX_LIGHTS);
         shaderProgram->setUniformValue("uNumLights", numLights);

         unsigned int lightIndex = 0;
         int shadowIndex = 0;
         int cubeShadowIndex = 0;
         for (int i = 0; i < numLights; ++i) {
            lights[i]->getLightComponent().draw(*shaderProgram, lightIndex++, shadowIndex, cubeShadowIndex);
         }
      }
   }
}

void Renderer::renderShadowMap(Scene &scene, SPtr<GameObject> light) {
   LightComponent &lightComponent = light->getLightComponent();

   SPtr<ShadowMap> shadowMap = lightComponent.getShadowMap();
   if (!shadowMap) {
      if (lightComponent.getLightType() == LightComponent::Point) {
         shadowMap = shadowMapManager->getFreeCubeMap();
      } else {
         shadowMap = shadowMapManager->getFreeStandardMap();
      }

      if (!shadowMap) {
         return;
      }

      lightComponent.setShadowMap(shadowMap);
   }

   shadowMap->enable();

   if (shadowMap->isCube()) {
      for (int i = 0; i < 6; ++i) {
         shadowMap->setActiveFace(i);
         renderShadowMapFace(scene, light, shadowMap->getShadowProgram(), i);
      }
   } else {
      renderShadowMapFace(scene, light, shadowMap->getShadowProgram());
   }

   shadowMap->disable();
}

void Renderer::renderShadowMapFace(Scene &scene, SPtr<GameObject> light, SPtr<ShaderProgram> shadowProgram, int face) {
   glClear(GL_DEPTH_BUFFER_BIT);
   LightComponent &lightComponent = light->getLightComponent();

   // Projection matrix
   shadowProgram->setUniformValue("uProjMatrix", lightComponent.getProjectionMatrix());

   // View matrix
   shadowProgram->setUniformValue("uViewMatrix", lightComponent.getViewMatrix(face));

   shadowProgram->setUniformValue("uLightDir", glm::normalize(lightComponent.getDirection()));

   RenderData renderData;
   renderData.setOverrideProgram(shadowProgram);

   // Objects
   const std::vector<SPtr<GameObject>> &gameObjects = scene.getObjects();
   for (SPtr<GameObject> gameObject : gameObjects) {
      if (gameObject == light) {
         continue;
      }

      gameObject->getGraphicsComponent().draw(renderData);
   }
}

void Renderer::renderFromCamera(Scene &scene, const GameObject &camera, const Viewport &viewport) {
   RenderData renderData;

   // Set up shaders
   const CameraComponent &cameraComponent = camera.getCameraComponent();
   const glm::mat4 &viewMatrix = cameraComponent.getViewMatrix();
   const glm::vec3 &cameraPosition = cameraComponent.getCameraPosition();
   const std::set<SPtr<ShaderProgram>> &shaderPrograms = scene.getShaderPrograms();
   for (SPtr<ShaderProgram> shaderProgram : shaderPrograms) {
      // Projection matrix
      shaderProgram->setUniformValue("uProjMatrix", projectionMatrix);

      // View matrix
      shaderProgram->setUniformValue("uViewMatrix", viewMatrix);

      // Camera position
      shaderProgram->setUniformValue("uCameraPos", cameraPosition, true);
   }

   // Sky
   SPtr<GameObject> sun = scene.getSun();
   if (sun) {
      glDisable(GL_DEPTH_TEST);
      skyRenderer.render(viewMatrix, projectionMatrix, viewport, glm::vec2((float)width, (float)height), sun);
      glEnable(GL_DEPTH_TEST);
   } else {
      glClear(GL_COLOR_BUFFER_BIT);
   }

   // Objects
   const std::vector<SPtr<GameObject>> &gameObjects = scene.getObjects();
   for (SPtr<GameObject> gameObject : gameObjects) {
      // Don't render the object that the camera is attached to
      if (&camera == gameObject.get()) {
         continue;
      }

      gameObject->getGraphicsComponent().draw(renderData);
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
