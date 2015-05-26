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

   bool isOnCooldown() const {
      return timeSinceLastUse < cooldown;
   }

   float getCooldownTime() const {
      return cooldown;
   }

   float getRemainingCooldownTime() const {
      return isOnCooldown() ? cooldown - timeSinceLastUse : 0.0f;
   }

   virtual void tick(const float dt) {
      timeSinceLastUse += dt;
   }

   virtual bool use() = 0;
};

#endif
