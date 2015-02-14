#ifndef ABILITY_H
#define ABILITY_H

class GameObject;

class Ability {
private:
   float cooldown;
   float timeSinceLastUse;

protected:
   GameObject &gameObject;

   void resetTimeSinceLastUse() {
      timeSinceLastUse = 0.0f;
   }

public:
   Ability(GameObject &gameObject, float cooldown)
      : cooldown(cooldown), timeSinceLastUse(cooldown), gameObject(gameObject) {
   }

   virtual ~Ability() {
   }

   bool isOnCooldown() {
      return timeSinceLastUse < cooldown;
   }

   virtual void tick(const float dt) {
      timeSinceLastUse += dt;
   }

   virtual void use() = 0;
};

#endif
