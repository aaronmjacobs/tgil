#include "Constants.h"
#include "ControllerInputDevice.h"
#include "FancyAssert.h"
#include "GLIncludes.h"
#include "InputComponent.h"
#include "InputHandler.h"
#include "KeyMouseInputDevice.h"

namespace {

const InputValues DEFAULT_INPUT_VALUES = { 0 };

} // namespace

InputHandler::InputHandler(GLFWwindow* const window)
   : window(window) {
   // Add any attached controllers
   for (int controller = GLFW_JOYSTICK_1; controller <= GLFW_JOYSTICK_LAST && inputDevices.size() < MAX_PLAYERS; ++controller) {
      if (glfwJoystickPresent(controller)) {
         inputDevices.push_back(std::make_shared<ControllerInputDevice>(window, controller));
      }
   }

   keyMouseInputDevice = std::make_shared<KeyMouseInputDevice>(window);
   // If there are less controllers than the max number of players, add the keyboard as the first player
   /*if (inputDevices.size() < MAX_PLAYERS) {
      inputDevices.insert(inputDevices.begin(), keyMouseInputDevice);

      // Hide / lock the mouse
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   }*/
}

InputHandler::~InputHandler() {
}

void InputHandler::pollInput() {
   // TODO Handle controllers being attached / detached
   ASSERT(inputDevices.size() <= MAX_PLAYERS, "More input devices than max number of players");

   inputValues.clear();
   for (SPtr<InputDevice> inputDevice : inputDevices) {
      inputValues.push_back(inputDevice->getInputValues());
   }

   keyMouseInputValues = keyMouseInputDevice->getInputValues();
}

const InputValues& InputHandler::getInputValues(int player) const {
   ASSERT(player >= 0 && player < inputValues.size(), "Invalid player number");
   if (player < 0 || player >= inputValues.size()) {
      return DEFAULT_INPUT_VALUES;
   }

   return inputValues.at(player);
}

const InputValues& InputHandler::getKeyMouseInputValues() const {
   return keyMouseInputValues;
}

int InputHandler::getNumDevices() const {
   return inputDevices.size();
}

double InputHandler::getMouseX() const {
   return keyMouseInputDevice->getMouseX();
}

double InputHandler::getMouseY() const {
   return keyMouseInputDevice->getMouseY();
}

bool InputHandler::isLeftMouseClicked() const {
   return keyMouseInputDevice->isLeftMouseClicked();
}

bool InputHandler::isRightMouseClicked() const {
   return keyMouseInputDevice->isRightMouseClicked();
}
