#include "ControllerInputDevice.h"
#include "FancyAssert.h"

#include <glm/glm.hpp>

#include <algorithm>

namespace {

#ifdef __APPLE__

const ControllerMap XBOX_360_CONTROLLER_MAP = { false, 0.7f, 0.2f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { 5, false, false }, { 4, false, false }, 11, -1, -1 };

const ControllerMap PS3_CONTROLLER_MAP = { false, 0.7f, 0.1f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { -1, false, false }, { -1, false, false }, 14, 9, 8 };

const ControllerMap PS4_CONTROLLER_MAP = { false, 0.7f, 0.1f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { 5, false, false }, { 4, false, false }, 1, -1, -1 };

#endif // __APPLE__

#ifdef __linux__

// TODO Figure out Linux bindings

const ControllerMap XBOX_360_CONTROLLER_MAP = { false, 0.7f, 0.2f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { 5, false, false }, { 4, false, false }, 11, -1, -1 };

const ControllerMap PS3_CONTROLLER_MAP = { false, 0.7f, 0.1f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { -1, false, false }, { -1, false, false }, 14, 9, 8 };

const ControllerMap PS4_CONTROLLER_MAP = { false, 0.7f, 0.1f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { 5, false, false }, { 4, false, false }, 1, -1, -1 };

#endif // __linux__

#ifdef _WIN32

const ControllerMap XBOX_360_CONTROLLER_MAP = { false, 0.7f, 0.2f, { 0, false, true }, { 1, false, true }, { 4, false, true }, { 3, false, true }, { 2, true, false }, { 2, false, false }, 0, -1, -1 };

// TODO PS3 not well supported, fall back to 360 - see if we can do better
const ControllerMap PS3_CONTROLLER_MAP = XBOX_360_CONTROLLER_MAP;

const ControllerMap PS4_CONTROLLER_MAP = { false, 0.7f, 0.1f, { 0, false, true }, { 1, false, true }, { 2, false, true }, { 3, false, true }, { 4, false, false }, { 5, false, false }, 1, -1, -1 };

#endif // _WIN32

bool stringContainsIgnoreCase(const std::string &haystack, const std::string &needle) {
   return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                       [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); })
           != haystack.end();
}

ControllerMap guessControllerMap(const std::string &name) {
   if (stringContainsIgnoreCase(name, "playstation") && stringContainsIgnoreCase(name, "3")) {
      return PS3_CONTROLLER_MAP;
   }
   if (stringContainsIgnoreCase(name, "360") || stringContainsIgnoreCase(name, "xbox")) {
      return XBOX_360_CONTROLLER_MAP;
   }
   if (stringContainsIgnoreCase(name, "wireless controller")) {
      return PS4_CONTROLLER_MAP;
   }

   return XBOX_360_CONTROLLER_MAP;
}

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

   float val = applyDeadzone(axes[axis.index], deadzone, axis.centerDeadzone) * (axis.invert ? -1.0f : 1.0f);

   // Take the square of the value to make small movements easier
   return val * val * glm::sign(val);
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
   : InputDevice(window), controller(controller), name(glfwGetJoystickName(controller)), map(guessControllerMap(name)) {
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

   inputValues.action = false;
   inputValues.jump = getButtonValue(buttons, buttonCount, map.jumpButton);
   inputValues.quit = false;
   inputValues.primaryAttack = getAxisValue(axes, axisCount, map.primaryTriggerAxis, map.deadzone) > 0.0f || getButtonValue(buttons, buttonCount, map.primaryTriggerButton);
   inputValues.secondaryAttack = getAxisValue(axes, axisCount, map.secondaryTriggerAxis, map.deadzone) > 0.0f || getButtonValue(buttons, buttonCount, map.secondaryTriggerButton);

   return inputValues;
}

void ControllerInputDevice::setControllerMap(const ControllerMap &map) {
   this->map = map;
}
