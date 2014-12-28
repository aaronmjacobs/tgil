#include "Constants.h"
#include "GLIncludes.h"
#include "LogHelper.h"
#include "Renderer.h"

#include <glm/glm.hpp>

#include <cstdlib>

namespace {

Renderer renderer;

void errorCallback(int error, const char* description) {
   LOG_FATAL("GLFW error " << error << ": " << description);
}

} // namespace

int main(int argc, char *argv[]) {
   LOG_INFO(PROJECT_NAME << " " << VERSION_TYPE << " " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_MICRO << "." << VERSION_BUILD);

   glfwSetErrorCallback(errorCallback);
   int glfwInitRes = glfwInit();
   if (!glfwInitRes) {
      LOG_FATAL("Unable to initialize GLFW");
   }

   GLFWwindow *window = glfwCreateWindow(1280, 720, PROJECT_NAME, NULL, NULL);
   if (!window) {
      LOG_FATAL("Unable to create GLFW window");
   }

   glfwMakeContextCurrent(window);
   glfwSwapInterval(1); // VSYNC

   int gladInitRes = gladLoadGL();
   if (!gladInitRes) {
      LOG_FATAL("Unable to initialize glad");
   }

   renderer.prepare();

   // Timing values
   double lastTime = glfwGetTime();
   const double dt = 1.0 / 60.0;
   double accumulator = dt;

   while (!glfwWindowShouldClose(window)) {
      // Calculate the frame time
      double now = glfwGetTime();
      double frameTime = glm::min(now - lastTime, 0.25); // Cap the frame time to .25 seconds to prevent spiraling
      lastTime = now;

      accumulator += frameTime;
      while (accumulator >= dt) {
         // TODO tick

         accumulator -= dt;
      }

      renderer.render();

      glfwSwapBuffers(window);

      glfwPollEvents();
   }

   glfwDestroyWindow(window);
   glfwTerminate();

   return EXIT_SUCCESS;
}
