#include "ControllerInputMap.h"

#include <glm/glm.hpp>

namespace {

const ControllerMap DEFAULT_CONTROLLER_MAP = { false, 1.0f, 0, 1, 2, 3, 5, 4, 1, 0 };

} // namespace

ControllerInputMap::ControllerInputMap(GLFWwindow* const window)
   : InputMap(window), map(DEFAULT_CONTROLLER_MAP) {
}

ControllerInputMap::~ControllerInputMap() {
}

const InputValues& ControllerInputMap::getInputValues(int player) {
   int buttonCount, axisCount;

   // TODO Map players onto controllers correctly
   const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + player, &buttonCount);
   const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + player, &axisCount);

   // If the controller can't be found, return the default values
   if (!buttons || !axes) {
      inputValues = { 0 };
      return inputValues;
   }

   // TODO Ensure no mapped index goes outside the bounds of the buttons / axes arrays

   // TODO Apply deadzone

   inputValues.moveForward = glm::max(0.0f, -axes[map.verticalMoveAxis]);
   inputValues.moveBackward = glm::max(0.0f, axes[map.verticalMoveAxis]);
   inputValues.moveLeft = glm::max(0.0f, -axes[map.horizontalMoveAxis]);
   inputValues.moveRight = glm::max(0.0f, axes[map.horizontalMoveAxis]);

   inputValues.lookY = -axes[map.verticalLookAxis];
   inputValues.lookX = axes[map.horizontalLookAxis];

   inputValues.action = buttons[map.actionButton] == GLFW_PRESS;
   inputValues.jump = buttons[map.jumpButton] == GLFW_PRESS;

   return inputValues;
}

void ControllerInputMap::setControllerMap(const ControllerMap &map) {
   this->map = map;
}
