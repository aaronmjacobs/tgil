#ifndef KEYBOARD_INPUT_DEVICE_H
#define KEYBOARD_INPUT_DEVICE_H

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

   int attackButton;
};

class KeyboardInputDevice : public InputDevice {
protected:
   KeyMouseMap map;
   double lastMouseX, lastMouseY;
   bool mouseInit;

public:
   KeyboardInputDevice(GLFWwindow* const window);

   virtual ~KeyboardInputDevice();

   virtual const InputValues& getInputValues();

   void setKeyMouseMap(const KeyMouseMap &map);
};

#endif
