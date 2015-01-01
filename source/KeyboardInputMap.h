#ifndef KEYBOARD_INPUT_MAP_H
#define KEYBOARD_INPUT_MAP_H

#include "InputMap.h"

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
};

class KeyboardInputMap : public InputMap {
protected:
   KeyMouseMap map;
   double lastMouseX, lastMouseY;
   bool mouseInit;

public:
   KeyboardInputMap(GLFWwindow* const window);
   virtual ~KeyboardInputMap();

   virtual const InputValues& getInputValues(int player);

   void setKeyMouseMap(const KeyMouseMap &map);
};

#endif
