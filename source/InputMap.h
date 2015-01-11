#ifndef INPUT_MAP_H
#define INPUT_MAP_H

#include "GLIncludes.h"

struct InputValues {
   // Range from 0 to 1
   float moveForward;
   float moveBackward;
   float moveLeft;
   float moveRight;

   // Full float range
   float lookX; // Negative = left, positive = right
   float lookY; // Negative = up, positive = down

   bool action;
   bool jump;
   bool quit;
};

class InputMap {
protected:
   GLFWwindow* const window;
   InputValues inputValues;

public:
   InputMap(GLFWwindow* const window)
      : window(window) {
   }

   virtual ~InputMap() {}

   virtual const InputValues& getInputValues(int player) = 0;
};

#endif
