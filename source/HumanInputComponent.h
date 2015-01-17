#ifndef HUMAN_INPUT_COMPONENT_H
#define HUMAN_INPUT_COMPONENT_H

#include "InputComponent.h"

class HumanInputComponent : public InputComponent, public std::enable_shared_from_this<HumanInputComponent> {
public:
   HumanInputComponent(GameObject &gameObject);

   virtual ~HumanInputComponent();

   virtual void init();
};

#endif
