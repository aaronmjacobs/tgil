#ifndef CONTROLLER_INPUT_DEVICE_H
#define CONTROLLER_INPUT_DEVICE_H

#include "InputDevice.h"

struct ControllerAxis {
   int index;
   bool invert;
   bool edgeDeadzone;
   bool centerDeadzone;
};

struct ControllerMap {
   bool invertYAxis;
   float lookSensitivity;

   ControllerAxis horizontalMoveAxis;
   ControllerAxis verticalMoveAxis;
   ControllerAxis horizontalLookAxis;
   ControllerAxis verticalLookAxis;
   ControllerAxis primaryTriggerAxis;
   ControllerAxis secondaryTriggerAxis;

   int actionButton;
   int jumpButton;
   int quitButton;
};

class ControllerInputDevice : public InputDevice {
protected:
   const int controller;
   ControllerMap map;

public:
   ControllerInputDevice(GLFWwindow* const window, const int controller);

   virtual ~ControllerInputDevice();

   virtual const InputValues& getInputValues();

   void setControllerMap(const ControllerMap &map);
};

#endif