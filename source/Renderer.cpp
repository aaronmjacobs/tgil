#include "CameraComponent.h"
#include "DebugDrawer.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GLIncludes.h"
#include "GraphicsComponent.h"
#include "LightComponent.h"
#include "LogHelper.h"
#include "PhysicsManager.h"
#include "Renderer.h"
#include "Scene.h"
#include "ShaderProgram.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

} // namespace

Renderer::Renderer() {
}

Renderer::~Renderer() {
}

void Renderer::init(float fov, int width, int height) {
   glClearColor(0.15f, 0.6f, 0.4f, 1.0f);

   // Depth Buffer Setup
   glClearDepth(1.0f);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   // Back face culling
   glCullFace(GL_BACK);

   this->fov = fov;
   onWindowSizeChange(width, height);

   debugRenderer.init();
}

void Renderer::onWindowSizeChange(int width, int height) {
   projectionMatrix = glm::perspective(glm::radians(fov), (float)width / height, 0.1f, 100.0f);
}

void Renderer::onMonitorChange() {
   // Handle recreation of any frame buffers
}

void Renderer::render(Scene &scene) {
   RUN_DEBUG(checkGLError();)

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   SPtr<GameObject> camera = scene.getCamera();
   if (!camera) {
      LOG_WARNING("Scene must have camera to render");
      return;
   }

   // Set up shaders
   const CameraComponent &cameraComponent = camera->getCameraComponent();
   const glm::mat4 &viewMatrix = cameraComponent.getViewMatrix();
   const glm::vec3 &cameraPosition = cameraComponent.getCameraPosition();
   const std::vector<SPtr<GameObject>>& lights = scene.getLights();
   const std::set<SPtr<ShaderProgram>>& shaderPrograms = scene.getShaderPrograms();
   for (SPtr<ShaderProgram> shaderProgram : shaderPrograms) {
      shaderProgram->use();

      // Projection matrix
      GLint uProjMatrix = shaderProgram->getUniform("uProjMatrix");
      glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

      // View matrix
      GLint uViewMatrix = shaderProgram->getUniform("uViewMatrix");
      glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

      // Camera position
      GLint uCameraPos = shaderProgram->getUniform("uCameraPos");
      glUniform3fv(uCameraPos, 1, glm::value_ptr(cameraPosition));

      // Lights
      glUniform1i(shaderProgram->getUniform("uNumLights"), lights.size());
      unsigned int lightIndex = 0;
      for (SPtr<GameObject> light : lights) {
         light->getLightComponent().draw(*shaderProgram, lightIndex++);
      }
   }

   // Objects
   const std::vector<SPtr<GameObject>>& gameObjects = scene.getObjects();
   for (SPtr<GameObject> gameObject : gameObjects) {
      gameObject->getGraphicsComponent().draw();
   }

   if (renderDebug) {
      DebugDrawer &debugDrawer = scene.getDebugDrawer();

      // Instruct Bullet to generate debug drawing data
      scene.getPhysicsManager()->debugDraw();

      // Render the data
      debugRenderer.render(debugDrawer, viewMatrix, projectionMatrix);

      // Clear the data
      debugDrawer.clear();
   }
}
