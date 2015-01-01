#ifndef HUMAN_INPUT_COMPONENT_H
#define HUMAN_INPUT_COMPONENT_H

#include "InputComponent.h"

class HumanInputComponent : public InputComponent {
protected:
   InputValues inputValues;

public:
   virtual ~HumanInputComponent();

   virtual void pollInput(const Context &context, GameObject &gameObject);

   virtual const InputValues& getInputValues(GameObject &gameObject);
};

#endif
