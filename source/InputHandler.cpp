#include "InputHandler.h"
#include "KeyboardInputMap.h"

InputHandler::InputHandler(GLFWwindow* const window)
   : window(window) {
   inputMap = std::make_shared<KeyboardInputMap>(window);
}

InputHandler::~InputHandler() {
}

const InputValues& InputHandler::getInputValues(int player) const {
   // TODO Handle multiple players
   return inputMap->getInputValues(player);
}

/*struct PS4Contoller {
public:
   // Buttons
   bool square, x, circle, triangle, l1, r1, l2, r2, share, options, l3, r3, ps, touch, up, right, down, left;

   // Axes
   float leftStickHorizontal, leftStickVertical, rightStickHorizontal, rightStickVertical, l2Axis, r2Axis;
};

const float DEADZONE = 0.1f;
const float AXIS_MIN = -1.0f;
const float AXIS_MAX = 1.0f;
const float AXIS_CENTER = AXIS_MIN + AXIS_MAX;

float applyDeadzone(float f, bool center) {
   if (f < AXIS_MIN + DEADZONE) {
      return AXIS_MIN;
   }

   if (f > AXIS_MAX - DEADZONE) {
      return AXIS_MAX;
   }

   if (center && f < AXIS_CENTER + DEADZONE && f > AXIS_CENTER - DEADZONE) {
      return AXIS_CENTER;
   }

   return f;
}

folly::Optional<PS4Controller> InputHandler::getController(int player) {
   int buttonCount, axisCount;

   const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + player, &buttonCount);
   const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + player, &axisCount);
   const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1 + player);

   if (!buttons || !axes) {
      //LOG_WARNING("Unable to detect controller for player " << player);
      return folly::none;
   }

   if (buttonCount != 18 || axisCount != 6) {
      //LOG_WARNING("Controller \"" << name << "\" (for player " << player << ") does not appear to be a PS4 controller");
      return folly::none;
   }

   PS4Controller controller;

   controller.square = buttons[0] == GLFW_PRESS;
   controller.x = buttons[1] == GLFW_PRESS;
   controller.circle = buttons[2] == GLFW_PRESS;
   controller.triangle = buttons[3] == GLFW_PRESS;
   controller.l1 = buttons[4] == GLFW_PRESS;
   controller.r1 = buttons[5] == GLFW_PRESS;
   controller.l2 = buttons[6] == GLFW_PRESS;
   controller.r2 = buttons[7] == GLFW_PRESS;
   controller.share = buttons[8] == GLFW_PRESS;
   controller.options = buttons[9] == GLFW_PRESS;
   controller.l3 = buttons[10] == GLFW_PRESS;
   controller.r3 = buttons[11] == GLFW_PRESS;
   controller.ps = buttons[12] == GLFW_PRESS;
   controller.touch = buttons[13] == GLFW_PRESS;
   controller.up = buttons[14] == GLFW_PRESS;
   controller.right = buttons[15] == GLFW_PRESS;
   controller.down = buttons[16] == GLFW_PRESS;
   controller.left = buttons[17] == GLFW_PRESS;

   controller.leftStickHorizontal = applyDeadzone(axes[0], true);
   controller.leftStickVertical = applyDeadzone(axes[1], true);
   controller.rightStickHorizontal = applyDeadzone(axes[2], true);
   controller.rightStickVertical = applyDeadzone(axes[3], true);
   controller.l2Axis = applyDeadzone(axes[4], false);
   controller.r2Axis = applyDeadzone(axes[5], false);

   return controller;
}
*/