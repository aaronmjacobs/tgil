#ifndef HUMAN_INPUT_COMPONENT_H
#define HUMAN_INPUT_COMPONENT_H

#include "InputComponent.h"

class HumanInputComponent : public InputComponent {
protected:
   InputValues inputValues;

public:
   HumanInputComponent(GameObject &gameObject);

   virtual ~HumanInputComponent();

   virtual void pollInput();

   virtual const InputValues& getInputValues();
};

#endif
