#ifndef CONTROLLER_INPUT_MAP_H
#define CONTROLLER_INPUT_MAP_H

#include "InputMap.h"

struct ControllerMap {
   bool invertYAxis;
   float lookSensitivity;

   int horizontalMoveAxis;
   int verticalMoveAxis;
   int horizontalLookAxis;
   int verticalLookAxis;
   int primaryTriggerAxis;
   int secondaryTriggerAxis;

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
