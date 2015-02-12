#ifndef PROJECTILE_LOGIC_COMPONENT_H
#define PROJECTILE_LOGIC_COMPONENT_H

#include "LogicComponent.h"

#include <functional>

typedef std::function<void(GameObject&, const float, const float)> ProjectileCollisionCallback;

class ProjectileLogicComponent : public LogicComponent {
protected:
   float lifeTime;
   ProjectileCollisionCallback collisionCallback;

public:
   ProjectileLogicComponent(GameObject &gameObject);

   virtual ~ProjectileLogicComponent();

   virtual void tick(const float dt);

   void setCollisionCallback(ProjectileCollisionCallback collisionCallback) {
      this->collisionCallback = collisionCallback;
   }
};

#endif
