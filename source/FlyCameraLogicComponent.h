#ifndef FLY_CAMERA_LOGIC_COMPONENT_H
#define FLY_CAMERA_LOGIC_COMPONENT_H

#include "LogicComponent.h"

class FlyCameraLogicComponent : public LogicComponent {
public:
   FlyCameraLogicComponent(GameObject &gameObject);

   virtual ~FlyCameraLogicComponent();

   virtual void tick(const float dt);
};

#endif
