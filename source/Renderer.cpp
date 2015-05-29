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
#include "PhysicsComponent.h"
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

const float FADE_TIME = 1.0f;
const float FADE_OUT_DELAY = TIME_TO_NEXT_LEVEL / 2.0f;

bool outside(const std::array<glm::vec3, 8> &aabbPoints, const glm::vec4 &plane) {
   for (const glm::vec3 &point : aabbPoints) {
      if (plane.x * point.x +
          plane.y * point.y +
          plane.z * point.z +
          plane.w >= 0.0f) {
         return false;
      }
   }

   return true;
}

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

FrustumChecker::FrustumChecker() {
}

FrustumChecker::~FrustumChecker() {
}

void FrustumChecker::updateFrustum(const glm::mat4 &viewProj) {
   int sign = 1;
   int t;

   for (int i = 0; i < planes.size(); i++) {
      t = i / 2;

      glm::vec4 &plane = planes[i];
      plane.x = viewProj[0][3] + sign * viewProj[0][t];
      plane.y = viewProj[1][3] + sign * viewProj[1][t];
      plane.z = viewProj[2][3] + sign * viewProj[2][t];
      plane.w = viewProj[3][3] + sign * viewProj[3][t];

      plane /= glm::length(glm::vec3(plane.x, plane.y, plane.z));

      if ((i % 2) == 0) {
         sign *= -1;
      }
   }
}

bool FrustumChecker::inFrustum(GameObject &gameObject) {
   if (!gameObject.getPhysicsComponent().getCollisionObject()) {
      // Can't check bounding box, assume we need to draw it
      return true;
   }

   AABB aabb = gameObject.getPhysicsComponent().getAABB();
   const glm::vec3 &min = aabb.min;
   const glm::vec3 &max = aabb.max;

   std::array<glm::vec3, 8> aabbPoints({{
      glm::vec3(min.x, min.y, min.z),
      glm::vec3(min.x, min.y, max.z),
      glm::vec3(min.x, max.y, min.z),
      glm::vec3(min.x, max.y, max.z),
      glm::vec3(max.x, min.y, min.z),
      glm::vec3(max.x, min.y, max.z),
      glm::vec3(max.x, max.y, min.z),
      glm::vec3(max.x, max.y, max.z)
   }});

   for (const glm::vec4 &plane : planes) {
      if (outside(aabbPoints, plane)) {
         return false;
      }
   }

   return true;
}

Renderer::Renderer()
   : clearFramesNeeded(0), renderDebug(false) {
}

Renderer::~Renderer() {
}

void Renderer::init(float fov, int width, int height, int windowWidth, int windowHeight) {
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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

   onWindowSizeChange(windowWidth, windowHeight);

   onFramebufferSizeChange(width, height);

   hudRenderer.init();

   postProcessRenderer.init();

   skyRenderer.init();

   textRenderer.init(pixelDensity);

   debugRenderer.init();
}

void Renderer::updatePixelDensity() {
   float newPixelDensity = (float)width / windowWidth;

   if (pixelDensity != newPixelDensity) {
      pixelDensity = newPixelDensity;
      textRenderer.onPixelDensityChange(pixelDensity);
   }
}

void Renderer::onFramebufferSizeChange(int width, int height) {
   this->width = width;
   this->height = height;
   clearFramesNeeded = 2; // Clear both the front and back buffers

   projectionMatrix = glm::perspective(glm::radians(fov), (float)width / height, 0.1f, 200.0f);

   updatePixelDensity();
}

void Renderer::onWindowSizeChange(int width, int height) {
   this->windowWidth = width;
   this->windowHeight = height;

   updatePixelDensity();
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

   glViewport(0, 0, width, height);

   renderFullscreenPost(scene);
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
   glm::mat4 proj(lightComponent.getProjectionMatrix());
   shadowProgram->setUniformValue("uProjMatrix", proj);

   // View matrix
   glm::mat4 view(lightComponent.getViewMatrix(face));
   shadowProgram->setUniformValue("uViewMatrix", view);

   shadowProgram->setUniformValue("uLightDir", glm::normalize(lightComponent.getDirection()));

   // View frustum
   frustumChecker.updateFrustum(proj * view);

   RenderData renderData(RenderState::Shadow);
   renderData.setOverrideProgram(shadowProgram);

   // Objects
   const std::vector<SPtr<GameObject>> &gameObjects = scene.getObjects();
   for (SPtr<GameObject> gameObject : gameObjects) {
      if (gameObject == light) {
         continue;
      }

      if (frustumChecker.inFrustum(*gameObject)) {
         shadowProgram->setUniformValue("uDisableNormalOffsetting", !gameObject->getGraphicsComponent().useNormalOffsetShadows(), true);
         gameObject->getGraphicsComponent().draw(renderData);
      }
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

   // Clear if needed
   SPtr<GameObject> sun = scene.getSun();
   if (!sun) {
      glClear(GL_COLOR_BUFFER_BIT);
   }

   // View frustum
   frustumChecker.updateFrustum(projectionMatrix * viewMatrix);

   // Opaque objects
   const std::vector<SPtr<GameObject>> &gameObjects = scene.getObjects();
   for (SPtr<GameObject> gameObject : gameObjects) {
      renderData.setRenderingCameraObject(&camera == gameObject.get());

      if (frustumChecker.inFrustum(*gameObject) && !gameObject->getGraphicsComponent().hasTransparency()) {
         gameObject->getGraphicsComponent().draw(renderData);
      }
   }

   // Sky
   if (sun) {
      skyRenderer.render(viewMatrix, projectionMatrix, viewport, glm::vec2((float)width, (float)height), sun);
   }

   // Transparent objects
   for (SPtr<GameObject> gameObject : gameObjects) {
      // Don't render the object that the camera is attached to
      if (&camera == gameObject.get()) {
         continue;
      }

      if (frustumChecker.inFrustum(*gameObject) && gameObject->getGraphicsComponent().hasTransparency()) {
         gameObject->getGraphicsComponent().draw(renderData);
      }
   }

   if (renderDebug) {
      renderDebugInfo(scene, viewMatrix);
   }

   renderCameraPost(scene, camera, viewport);
}

void Renderer::renderCameraPost(Scene &scene, const GameObject &camera, const Viewport &viewport) {
   PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&camera.getLogicComponent());

   if (!playerLogic) {
      return;
   }

   glDisable(GL_DEPTH_TEST);

   hudRenderer.render(*playerLogic, width, height);

   int playerNum = playerLogic->getPlayerNum();
   if (scene.getGameState().hasWinner() && scene.getGameState().getWinner() == playerNum) {
      float runningTime = Context::getInstance().getRunningTime() * 5.0f;
      float timeSinceEnd = scene.getTimeSinceEnd();
      float offset = glm::pi<float>() * 2.0f / 3.0f;
      float red = (glm::sin(runningTime) + 1.0f) / 2.0f;
      float green = (glm::sin(runningTime + offset) + 1.0f) / 2.0f;
      float blue = (glm::sin(runningTime + offset * 2.0f) + 1.0f) / 2.0f;
      float opacity = 0.5f * glm::smoothstep(0.0f, 0.5f, timeSinceEnd);
      postProcessRenderer.render(opacity, glm::vec3(red, green, blue));
   }

   if (!playerLogic->isAlive()) {
      float opacity = 0.75f * glm::smoothstep(0.0f, 1.0f, playerLogic->timeSinceDeath());
      postProcessRenderer.render(opacity, glm::vec3(0.0f));
   }

   renderScore(scene, *playerLogic, viewport);

   glEnable(GL_DEPTH_TEST);
}

void Renderer::renderScore(Scene &scene, const PlayerLogicComponent &playerLogic, const Viewport &viewport) {
   const GameSession &session = Context::getInstance().getGameSession();
   if (!session.currentLevelEnded) {
      return;
   }

   int playerNum = playerLogic.getPlayerNum();
   ASSERT(playerNum >= 0 && playerNum < session.players.size(), "Invalid player number: %d", playerNum);

   float timeSinceEnd = scene.getTimeSinceEnd();
   int winner = scene.getGameState().getWinner();

   int score = session.players[playerNum].score;
   FontType type = FontType::Medium;
   std::string suffix;
   if (playerNum == winner) {
      if (timeSinceEnd < 1.25f) {
         --score;

         if (timeSinceEnd > 0.75f) {
            suffix += "+";

            if (timeSinceEnd > 1.0f) {
               suffix += "+";
            }
         }
      } else {
         type = FontType::LargeNumber;
      }
   }
   std::string text(std::to_string(score) + suffix);

   textRenderer.renderImmediate(viewport.width, viewport.height, 0.5f, 0.5f, text, type);
}

void Renderer::renderFullscreenPost(Scene &scene) {
   glDisable(GL_DEPTH_TEST);

   float opacity = 0.0f;

   // Fade in
   float timeSinceStart = scene.getTimeSinceStart();
   if (timeSinceStart < 1.0f) {
      opacity = 1.0f - glm::smoothstep(0.0f, FADE_TIME, timeSinceStart);
   }

   // Fade out
   float timeSinceEnd = scene.getTimeSinceEnd();
   if (timeSinceEnd - FADE_OUT_DELAY > 0.0f) {
      opacity = glm::smoothstep(0.0f, FADE_TIME, timeSinceEnd - FADE_OUT_DELAY);
   }

   if (opacity != 0.0f) {
      postProcessRenderer.render(opacity, glm::vec3(0.0f));
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

SPtr<Texture> Renderer::renderTextToTexture(const std::string &text, Resolution *resolution) {
   return textRenderer.renderToTexture(text, resolution);
}
