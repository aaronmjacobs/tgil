#include "ControllerInputDevice.h"
#include "FancyAssert.h"

#include <glm/glm.hpp>

namespace {

const ControllerMap DEFAULT_CONTROLLER_MAP = { false, 1.0f, 0.1f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { 5, false, false }, { 4, false, false }, 0, 7, 1, 12 };

const float AXIS_MIN = -1.0f;
const float AXIS_MAX = 1.0f;
const float AXIS_CENTER = 0.0f;

float applyEdgeDeadzone(float f, float minVal, float maxVal, float deadzone) {
   float center = (minVal + maxVal) / 2.0f;
   float fullScale = maxVal - minVal;
   float deadzonedScale = (maxVal - deadzone) - (minVal + deadzone);
   if (deadzonedScale == 0.0f) {
      return f;
   }

   float scale = fullScale / deadzonedScale;
   return glm::clamp(((f - center) * scale) + center, minVal, maxVal);
}

float applyDeadzone(float f, float deadzone, bool center) {
   float minVal = AXIS_MIN;
   float maxVal = AXIS_MAX;

   if (center) {
      if (f > AXIS_CENTER) {
         minVal = AXIS_CENTER;
      } else {
         maxVal = AXIS_CENTER;
      }
   }

   return applyEdgeDeadzone(f, minVal, maxVal, deadzone);
}

float getAxisValue(const float* axes, const ControllerAxis &axis, const float deadzone) {
   return applyDeadzone(axes[axis.index], deadzone, axis.centerDeadzone) * (axis.invert ? -1.0f : 1.0f);
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

   inputValues.moveForward = glm::max(0.0f, -getAxisValue(axes, map.verticalMoveAxis, map.deadzone));
   inputValues.moveBackward = glm::max(0.0f, getAxisValue(axes, map.verticalMoveAxis, map.deadzone));
   inputValues.moveLeft = glm::max(0.0f, -getAxisValue(axes, map.horizontalMoveAxis, map.deadzone));
   inputValues.moveRight = glm::max(0.0f, getAxisValue(axes, map.horizontalMoveAxis, map.deadzone));

   inputValues.lookY = getAxisValue(axes, map.verticalLookAxis, map.deadzone) * map.lookSensitivity * (map.invertYAxis ? -1.0f : 1.0f);
   inputValues.lookX = getAxisValue(axes, map.horizontalLookAxis, map.deadzone) * map.lookSensitivity;

   inputValues.action = buttons[map.actionButton] == GLFW_PRESS;
   inputValues.jump = buttons[map.jumpButton] == GLFW_PRESS;
   inputValues.quit = buttons[map.quitButton] == GLFW_PRESS;
   inputValues.attack = buttons[map.attackButton] == GLFW_PRESS;

   return inputValues;
}

void ControllerInputDevice::setControllerMap(const ControllerMap &map) {
   this->map = map;
}
