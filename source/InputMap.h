#ifndef INPUT_MAP_H
#define INPUT_MAP_H

#include "GLIncludes.h"

struct InputValues {
   float moveForward;
   float moveBackward;
   float moveLeft;
   float moveRight;

   float lookX;
   float lookY;

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
