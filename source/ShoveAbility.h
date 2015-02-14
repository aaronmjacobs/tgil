#ifndef SHOVE_ABILITY_H
#define SHOVE_ABILITY_H

#include "Ability.h"

class ShoveAbility : public Ability {
public:
   ShoveAbility(GameObject &gameObject);

   virtual ~ShoveAbility();

   virtual void use();
};

#endif
