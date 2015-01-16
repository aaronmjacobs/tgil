#ifndef LOGIC_COMPONENT_H
#define LOGIC_COMPONENT_H

#include "Component.h"

class LogicComponent : public Component {
public:
   LogicComponent(GameObject &gameObject)
      : Component(gameObject) {}

   virtual ~LogicComponent() {}

   virtual void tick(const float dt) = 0;
};

class NullLogicComponent : public LogicComponent {
public:
   NullLogicComponent(GameObject &gameObject)
      : LogicComponent(gameObject) {}

   virtual ~NullLogicComponent() {}

   virtual void tick(const float dt) {}
};

#endif
