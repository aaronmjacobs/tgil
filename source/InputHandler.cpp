#include "ControllerInputMap.h"
#include "FancyAssert.h"
#include "GLIncludes.h"
#include "InputHandler.h"
#include "KeyboardInputMap.h"

namespace {

const InputValues DEFAULT_INPUT_VALUES = { 0 };

} // namespace

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

void InputHandler::pollInput() {
   // TODO Determine number of players
   inputValues.resize(1);

   for (int i = 0; i < inputValues.size(); ++i) {
      inputValues[i] = inputMap->getInputValues(i);
   }
}

const InputValues& InputHandler::getInputValues(int player) const {
   ASSERT(player >= 0 && player < inputValues.size(), "Invalid player number");
   if (player < 0 || player >= inputValues.size()) {
      return DEFAULT_INPUT_VALUES;
   }

   return inputValues.at(player);
}
