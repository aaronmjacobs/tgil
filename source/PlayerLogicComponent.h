#ifndef PLAYER_LOGIC_COMPONENT_H
#define PLAYER_LOGIC_COMPONENT_H

#include "LogicComponent.h"

class PlayerLogicComponent : public LogicComponent {
protected:
   bool wasJumpingLastFrame;

public:
   PlayerLogicComponent(GameObject &gameObject);

   virtual ~PlayerLogicComponent();

   virtual void tick(const float dt);
};

#endif
