#ifndef KEYMOUSE_INPUT_DEVICE_H
#define KEYMOUSE_INPUT_DEVICE_H

#include "InputDevice.h"

struct KeyMouseMap {
   bool invertMouseY;
   bool swapMouseButtons;
   float mouseSensitivity;

   int moveForwardKey;
   int moveBackwardKey;
   int moveLeftKey;
   int moveRightKey;
   int actionKey;
   int jumpKey;
   int quitKey;

   int primaryAttackButton;
   int secondaryAttackButton;
};

class KeyMouseInputDevice : public InputDevice {
protected:
   KeyMouseMap map;
   double lastMouseX, lastMouseY;
   bool leftMouseButton, rightMouseButton;
   bool mouseInit;

public:
   KeyMouseInputDevice(GLFWwindow* const window);

   virtual ~KeyMouseInputDevice();

   virtual InputValues getInputValues();

   void setKeyMouseMap(const KeyMouseMap &map);

   double getMouseX() const {
      return lastMouseX;
   }

   double getMouseY() const {
      return lastMouseY;
   }

   bool isLeftMouseClicked() const {
      return leftMouseButton;
   }

   bool isRightMouseClicked() const {
      return rightMouseButton;
   }
};

#endif
