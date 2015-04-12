#ifndef SUN_LOGIC_COMPONENT_H
#define SUN_LOGIC_COMPONENT_H

#include "LogicComponent.h"

class SunLogicComponent : public LogicComponent {
public:
   SunLogicComponent(GameObject &gameObject);

   virtual ~SunLogicComponent();

   virtual void tick(const float dt);
};

#endif
