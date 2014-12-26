#include "Constants.h"
#include "GLIncludes.h"
#include "LogHelper.h"

#include <cstdlib>

int main(int argc, char *argv[]) {
   LOG_INFO(VERSION_TYPE << " " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_MICRO << "." << VERSION_BUILD, PROJECT_NAME);

   int glfwInitRes = glfwInit();
   if (!glfwInitRes) {
      LOG_FATAL("Unable to initialize GLFW", "GLFW Error");
   }

   GLFWwindow *window = glfwCreateWindow(1280, 720, PROJECT_NAME, NULL, NULL);
   if (!window) {
      LOG_FATAL("Unable to create GLFW window", "GLFW Error");
   }

   glfwMakeContextCurrent(window);

   // Load glad
   int gladInitRes = gladLoadGL();
   if (!gladInitRes) {
      LOG_FATAL("Unable to initialize glad", "glad Error");
   }

   // Game loop
   while (!glfwWindowShouldClose(window)) {
      // TODO Tick

      // TODO render

      glfwSwapBuffers(window);

      // Poll for events
      glfwPollEvents();
   }

   return EXIT_SUCCESS;
}
