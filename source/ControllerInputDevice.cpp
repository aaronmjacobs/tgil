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

float getAxisValue(const float *axes, const int axisCount, const ControllerAxis &axis, const float deadzone) {
   // Make sure the mapped index is valid
   if (axis.index < 0 || axis.index >= axisCount) {
      return 0.0f;
   }

   return applyDeadzone(axes[axis.index], deadzone, axis.centerDeadzone) * (axis.invert ? -1.0f : 1.0f);
}

bool getButtonValue(const unsigned char *buttons, const int buttonCount, const int button) {
   // Make sure the mapped index is valid
   if (button < 0 || button >= buttonCount) {
      return false;
   }

   return buttons[button] == GLFW_PRESS;
}

} // namespace

ControllerInputDevice::ControllerInputDevice(GLFWwindow* const window, const int controller)
   : InputDevice(window), controller(controller), map(DEFAULT_CONTROLLER_MAP) {
}

ControllerInputDevice::~ControllerInputDevice() {
}

InputValues ControllerInputDevice::getInputValues() {
   InputValues inputValues = { 0 };

   // If the controller isn't present, return the default values
   if (!glfwJoystickPresent(controller)) {
      return inputValues;
   }

   int buttonCount, axisCount;
   const unsigned char* buttons = glfwGetJoystickButtons(controller, &buttonCount);
   const float* axes = glfwGetJoystickAxes(controller, &axisCount);

   // If the buttons or axes can't be accessed, return the default values
   if (!buttons || !axes) {
      return inputValues;
   }

   float verticalMove = getAxisValue(axes, axisCount, map.verticalMoveAxis, map.deadzone);
   float horizontalMove = getAxisValue(axes, axisCount, map.horizontalMoveAxis, map.deadzone);
   inputValues.moveForward = glm::max(0.0f, -verticalMove);
   inputValues.moveBackward = glm::max(0.0f, verticalMove);
   inputValues.moveLeft = glm::max(0.0f, -horizontalMove);
   inputValues.moveRight = glm::max(0.0f, horizontalMove);

   inputValues.lookY = getAxisValue(axes, axisCount, map.verticalLookAxis, map.deadzone) * map.lookSensitivity * (map.invertYAxis ? -1.0f : 1.0f);
   inputValues.lookX = getAxisValue(axes, axisCount, map.horizontalLookAxis, map.deadzone) * map.lookSensitivity;

   inputValues.action = getButtonValue(buttons, buttonCount, map.actionButton);
   inputValues.jump = getButtonValue(buttons, buttonCount, map.jumpButton);
   inputValues.quit = getButtonValue(buttons, buttonCount, map.quitButton);
   inputValues.attack = getButtonValue(buttons, buttonCount, map.attackButton);

   return inputValues;
}

void ControllerInputDevice::setControllerMap(const ControllerMap &map) {
   this->map = map;
}
