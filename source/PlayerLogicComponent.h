#ifndef PLAYER_LOGIC_COMPONENT_H
#define PLAYER_LOGIC_COMPONENT_H

#include "LogicComponent.h"

class PlayerLogicComponent : public LogicComponent {
protected:
   bool wasJumpingLastFrame;

   void handleOrientation(const float dt, const InputValues &inputValues);

   void handleMovement(const float dt, const InputValues &inputValues, SPtr<Scene> scene);

public:
   PlayerLogicComponent(GameObject &gameObject);

   virtual ~PlayerLogicComponent();

   virtual void tick(const float dt);
};

#endif
