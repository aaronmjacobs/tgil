#include "Context.h"
#include "GameObject.h"
#include "GLIncludes.h"
#include "GraphicsComponent.h"
#include "LightComponent.h"
#include "LogHelper.h"
#include "Renderer.h"
#include "Scene.h"
#include "ShaderProgram.h"

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

void Renderer::prepare() {
   glClearColor(0.15f, 0.6f, 0.4f, 1.0f);

   // Depth Buffer Setup
   glClearDepth(1.0f);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   // Back face culling
   glCullFace(GL_BACK);
}

void Renderer::render(const Context &context) {
   checkGLError(); // TODO Only in debug builds

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   Scene &scene = context.getScene();

   // Lights
   const std::vector<SPtr<GameObject>>& lights = scene.getLights();
   const std::set<SPtr<ShaderProgram>>& shaderPrograms = scene.getShaderPrograms();
   for (SPtr<ShaderProgram> shaderProgram : shaderPrograms) {
      shaderProgram->use();
      glUniform1i(shaderProgram->getUniform("uNumLights"), lights.size());

      unsigned int lightIndex = 0;
      for (SPtr<GameObject> light : lights) {
         LightComponent &lightComponent = light->getLightComponent();
         lightComponent.draw(*light, *shaderProgram, lightIndex++);
      }
   }

   // Objects
   const std::vector<SPtr<GameObject>>& gameObjects = scene.getObjects();
   for (SPtr<GameObject> gameObject : gameObjects) {
      gameObject->getGraphicsComponent().draw(*gameObject);
   }
}
