#include "Constants.h"
#include "GLIncludes.h"

#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
   printf("%s %s %d.%d.%d.%d\n", PROJECT_NAME, VERSION_TYPE, VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, VERSION_BUILD);

   int glfwInitRes = glfwInit();
   if (!glfwInitRes) {
      return EXIT_FAILURE;
   }

   GLFWwindow *window = glfwCreateWindow(1280, 720, PROJECT_NAME, NULL, NULL);
   if (!window) {
      return EXIT_FAILURE;
   }

   glfwMakeContextCurrent(window);

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
