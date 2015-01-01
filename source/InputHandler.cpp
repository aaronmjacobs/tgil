#include "ControllerInputMap.h"
#include "GLIncludes.h"
#include "InputHandler.h"
#include "KeyboardInputMap.h"

InputHandler::InputHandler(GLFWwindow* const window)
   : window(window) {
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
   return inputMap->getInputValues(player);
}
