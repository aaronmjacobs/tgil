#ifndef CONTROLLER_INPUT_DEVICE_H
#define CONTROLLER_INPUT_DEVICE_H

#include "InputDevice.h"

struct ControllerAxis {
   int index;
   bool invert;
   bool centerDeadzone;
};

struct ControllerMap {
   bool invertYAxis;
   float lookSensitivity;
   float deadzone;

   ControllerAxis horizontalMoveAxis;
   ControllerAxis verticalMoveAxis;
   ControllerAxis horizontalLookAxis;
   ControllerAxis verticalLookAxis;
   ControllerAxis primaryTriggerAxis;
   ControllerAxis secondaryTriggerAxis;

   int jumpButton;
};

class ControllerInputDevice : public InputDevice {
protected:
   const int controller;
   ControllerMap map;

public:
   ControllerInputDevice(GLFWwindow* const window, const int controller);

   virtual ~ControllerInputDevice();

   virtual InputValues getInputValues();

   void setControllerMap(const ControllerMap &map);
};

#endif
