#ifndef CONTROLLER_INPUT_MAP_H
#define CONTROLLER_INPUT_MAP_H

#include "InputMap.h"

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
};

class ControllerInputMap : public InputMap {
protected:
   ControllerMap map;

public:
   ControllerInputMap(GLFWwindow* const window);
   virtual ~ControllerInputMap();

   virtual const InputValues& getInputValues(int player);

   void setControllerMap(const ControllerMap &map);
};

#endif
