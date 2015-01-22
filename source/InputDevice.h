#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

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
   bool attack;
   bool jump;
   bool quit;
};

class InputDevice {
protected:
   GLFWwindow* const window;
   InputValues inputValues;

public:
   InputDevice(GLFWwindow* const window)
      : window(window) {
   }

   virtual ~InputDevice() {}

   virtual const InputValues& getInputValues() = 0;
};

#endif
