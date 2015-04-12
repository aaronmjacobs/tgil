#include "Constants.h"
#include "Context.h"
#include "GLIncludes.h"
#include "LogHelper.h"
#include "OSUtils.h"
#include "Renderer.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include <cstdlib>

namespace {

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float FOV = 70.0f;

void errorCallback(int error, const char* description) {
   LOG_FATAL("GLFW error " << error << ": " << description);
}

void focusCallback(GLFWwindow* window, GLint focused) {
   if (focused) {
      Context::getInstance().onWindowFocusGained();
   }
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
   Context::getInstance().getRenderer().onFramebufferSizeChange(width, height);
}

} // namespace

int main(int argc, char *argv[]) {
   LOG_INFO(PROJECT_NAME << " " << VERSION_TYPE << " " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_MICRO << "." << VERSION_BUILD);

   if (!OSUtils::fixWorkingDirectory()) {
      LOG_ERROR("Unable to fix working directory");
   }

   glfwSetErrorCallback(errorCallback);
   int glfwInitRes = glfwInit();
   if (!glfwInitRes) {
      LOG_FATAL("Unable to initialize GLFW");
   }

   // Set hints to use OpenGL 3.3
   // TODO Fix for Windows / Linux
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PROJECT_NAME, NULL, NULL);
   if (!window) {
      glfwTerminate();
      LOG_FATAL("Unable to create GLFW window");
   }

   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // VSYNC

   int gladInitRes = gladLoadGL();
   if (!gladInitRes) {
      glfwDestroyWindow(window);
      glfwTerminate();
      LOG_FATAL("Unable to initialize glad");
   }

   Context::load(window);
   Context &context = Context::getInstance();
   Renderer &renderer = context.getRenderer();

   int framebufferWidth, framebufferHeight;
   glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
   renderer.init(FOV, framebufferWidth, framebufferHeight);

   glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
   glfwSetWindowFocusCallback(window, focusCallback);

   // Timing values
   const double dt = 1.0 / 60.0;
   double lastTime = glfwGetTime();
   // Make sure we tick at least once before rendering, to allow things to be set up
   double accumulator = dt;

   while (!glfwWindowShouldClose(window)) {
      // Calculate the frame time
      double now = glfwGetTime();
      double frameTime = glm::min(now - lastTime, 0.25); // Cap the frame time to .25 seconds to prevent spiraling
      lastTime = now;

      accumulator += frameTime;
      while (accumulator >= dt) {
         context.tick(dt);

         accumulator -= dt;
      }

      renderer.render(context.getScene());

      glfwSwapBuffers(window);

      glfwPollEvents();
   }

   glfwDestroyWindow(window);
   glfwTerminate();

   return EXIT_SUCCESS;
}
