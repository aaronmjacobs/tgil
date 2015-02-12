#ifndef THROW_ABILITY_H
#define THROW_ABILITY_H

#include "Ability.h"

class ThrowAbility : public Ability {
public:
   ThrowAbility(GameObject &gameObject);

   virtual ~ThrowAbility();

   virtual void use();
};

#endif
