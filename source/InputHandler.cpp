#include "ControllerInputMap.h"
#include "GLIncludes.h"
#include "InputHandler.h"
#include "KeyboardInputMap.h"

#include <boxer/boxer.h>

InputHandler::InputHandler(GLFWwindow* const window)
   : window(window) {
   // Hide / lock the mouse
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
      inputMap = std::make_shared<ControllerInputMap>(window);
   } else {
      inputMap = std::make_shared<KeyboardInputMap>(window);
   }
}

InputHandler::~InputHandler() {
}

const InputValues& InputHandler::getInputValues(int player) const {
   // TODO Handle multiple players
   const InputValues &inputValues = inputMap->getInputValues(player);

   if (inputValues.quit) {
      // TODO Prevent cursor from being locked for all message boxes
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      boxer::Selection selection = boxer::show("Do you want to quit?", "Quit", boxer::Style::Question, boxer::Buttons::YesNo);
      if (selection == boxer::Selection::Yes) {
         glfwSetWindowShouldClose(window, true);
      } else {
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
   }

   return inputValues;
}
