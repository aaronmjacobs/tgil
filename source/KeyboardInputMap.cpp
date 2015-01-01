#include "GLIncludes.h"
#include "KeyboardInputMap.h"

namespace {

const float SENSITIVITY_ADJUST = 0.1f;
const KeyMouseMap DEFAULT_KEY_MOUSE_MAP { false, false, 1.0f, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_E, GLFW_KEY_SPACE };

} // namespace

KeyboardInputMap::KeyboardInputMap(GLFWwindow* const window)
   : InputMap(window), map(DEFAULT_KEY_MOUSE_MAP), mouseInit(false) {
   glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
}

KeyboardInputMap::~KeyboardInputMap() {
}

const InputValues& KeyboardInputMap::getInputValues(int player) {
   inputValues.moveForward = glfwGetKey(window, map.moveForwardKey) == GLFW_PRESS ? 1.0f : 0.0f;
   inputValues.moveBackward = glfwGetKey(window, map.moveBackwardKey) == GLFW_PRESS ? 1.0f : 0.0f;
   inputValues.moveLeft = glfwGetKey(window, map.moveLeftKey) == GLFW_PRESS ? 1.0f : 0.0f;
   inputValues.moveRight = glfwGetKey(window, map.moveRightKey) == GLFW_PRESS ? 1.0f : 0.0f;

   double mouseX, mouseY;
   glfwGetCursorPos(window, &mouseX, &mouseY);
   if (mouseInit) {
      inputValues.lookY = (float)(mouseY - lastMouseY) * map.mouseSensitivity * SENSITIVITY_ADJUST * (map.invertMouseY ? -1.0f : 1.0f);
      inputValues.lookX = (float)(mouseX - lastMouseX) * map.mouseSensitivity * SENSITIVITY_ADJUST;
   } else {
      inputValues.lookY = inputValues.lookX = 0.0f;

      // Don't count input until we have received actual mouse movement, to prevent the view from "jumping"
      mouseInit = mouseX != lastMouseX || mouseY != lastMouseY;
   }

   lastMouseX = mouseX;
   lastMouseY = mouseY;

   inputValues.action = glfwGetKey(window, map.actionKey) == GLFW_PRESS;
   inputValues.jump = glfwGetKey(window, map.jumpKey) == GLFW_PRESS;

   return inputValues;
}

void KeyboardInputMap::setKeyMouseMap(const KeyMouseMap &map) {
   this->map = map;
}
