#include "ControllerInputDevice.h"
#include "FancyAssert.h"
#include "GLIncludes.h"
#include "InputComponent.h"
#include "InputHandler.h"
#include "KeyboardInputDevice.h"

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

   // If there are less controllers than the max number of players, add the keyboard as the first player
   if (inputDevices.size() < MAX_PLAYERS) {
      inputDevices.insert(inputDevices.begin(), std::make_shared<KeyboardInputDevice>(window));

      // Hide / lock the mouse
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   }
}

InputHandler::~InputHandler() {
}

void InputHandler::registerInputComponent(SPtr<InputComponent> inputComponent) {
   ASSERT(inputComponents.size() <= inputDevices.size(), "More InputComponents than devices");
   inputComponents.push_back(inputComponent);
}

void InputHandler::pollInput() {
   // TODO Handle controllers being attached / detached
   ASSERT(inputDevices.size() <= MAX_PLAYERS, "More input devices than max number of players");
   ASSERT(inputComponents.size() <= inputDevices.size(), "More InputComponents than devices");

   inputValues.clear();
   for (SPtr<InputDevice> inputDevice : inputDevices) {
      inputValues.push_back(inputDevice->getInputValues());
   }

   int i = 0;
   for (std::vector<WPtr<InputComponent>>::iterator itr = inputComponents.begin(); itr != inputComponents.end();) {
      SPtr<InputComponent> inputComponent = itr->lock();
      if (inputComponent) {
         if (i < inputValues.size()) {
            inputComponent->setInputValues(inputValues[i]);
         }

         ++i;
         ++itr;
      } else {
         itr = inputComponents.erase(itr);
      }
   }
}

const InputValues& InputHandler::getInputValues(int player) const {
   ASSERT(player >= 0 && player < inputValues.size(), "Invalid player number");
   if (player < 0 || player >= inputValues.size()) {
      return DEFAULT_INPUT_VALUES;
   }

   return inputValues.at(player);
}
