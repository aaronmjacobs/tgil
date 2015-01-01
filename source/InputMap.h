#ifndef INPUT_MAP_H
#define INPUT_MAP_H

#include "GLIncludes.h"

struct InputValues {
   // Range from 0 to 1
   float moveForward;
   float moveBackward;
   float moveLeft;
   float moveRight;

   // Range from -1 to 1
   float lookX; // -1 = left, 1 = right
   float lookY; // -1 = up, 1 = down

   bool action;
   bool jump;
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
