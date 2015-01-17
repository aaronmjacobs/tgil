#include "ControllerInputDevice.h"
#include "FancyAssert.h"

#include <glm/glm.hpp>

namespace {

const ControllerMap DEFAULT_CONTROLLER_MAP = { false, 1.0f, { 0, false, true, true }, { 1, false, true, true }, { 2, false, true, true }, { 3, false, true, true }, { 5, false, true, false }, { 4, false, true, false }, 0, 1, 12 };

// TODO Make deadzone configurable
const float DEADZONE = 0.1f;
const float AXIS_MIN = -1.0f;
const float AXIS_MAX = 1.0f;
const float AXIS_CENTER = AXIS_MIN + AXIS_MAX;

float applyDeadzone(float f, bool edge, bool center) {
   if (edge) {
      if (f < AXIS_MIN + DEADZONE) {
         return AXIS_MIN;
      }
      if (f > AXIS_MAX - DEADZONE) {
         return AXIS_MAX;
      }
   }

   if (center && f < AXIS_CENTER + DEADZONE && f > AXIS_CENTER - DEADZONE) {
      return AXIS_CENTER;
   }

   return f;
}

float getAxisValue(const float* axes, const ControllerAxis &axis) {
   return applyDeadzone(axes[axis.index], axis.edgeDeadzone, axis.centerDeadzone) * (axis.invert ? -1.0f : 1.0f);
}

} // namespace

ControllerInputDevice::ControllerInputDevice(GLFWwindow* const window, const int controller)
   : InputDevice(window), controller(controller), map(DEFAULT_CONTROLLER_MAP) {
}

ControllerInputDevice::~ControllerInputDevice() {
}

const InputValues& ControllerInputDevice::getInputValues() {
   int buttonCount, axisCount;

   const unsigned char* buttons = glfwGetJoystickButtons(controller, &buttonCount);
   const float* axes = glfwGetJoystickAxes(controller, &axisCount);

   // If the controller can't be found, return the default values
   if (!buttons || !axes) {
      inputValues = { 0 };
      return inputValues;
   }

   // TODO Ensure no mapped index goes outside the bounds of the buttons / axes arrays

   inputValues.moveForward = glm::max(0.0f, -getAxisValue(axes, map.verticalMoveAxis));
   inputValues.moveBackward = glm::max(0.0f, getAxisValue(axes, map.verticalMoveAxis));
   inputValues.moveLeft = glm::max(0.0f, -getAxisValue(axes, map.horizontalMoveAxis));
   inputValues.moveRight = glm::max(0.0f, getAxisValue(axes, map.horizontalMoveAxis));

   inputValues.lookY = getAxisValue(axes, map.verticalLookAxis) * map.lookSensitivity * (map.invertYAxis ? -1.0f : 1.0f);
   inputValues.lookX = getAxisValue(axes, map.horizontalLookAxis) * map.lookSensitivity;

   inputValues.action = buttons[map.actionButton] == GLFW_PRESS;
   inputValues.jump = buttons[map.jumpButton] == GLFW_PRESS;
   inputValues.quit = buttons[map.quitButton] == GLFW_PRESS;

   return inputValues;
}

void ControllerInputDevice::setControllerMap(const ControllerMap &map) {
   this->map = map;
}
