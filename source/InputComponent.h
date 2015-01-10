#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "Component.h"
#include "InputHandler.h"

class InputComponent : public Component {
public:
   InputComponent(GameObject &gameObject)
      : Component(gameObject) {}

   virtual ~InputComponent() {}

   virtual void pollInput() = 0;

   virtual const InputValues& getInputValues() = 0;
};

class NullInputComponent : public InputComponent {
private:
   const InputValues INPUT_VALUES;

public:
   NullInputComponent(GameObject &gameObject)
      : InputComponent(gameObject), INPUT_VALUES({ 0 }) {}

   virtual ~NullInputComponent() {}

   virtual void pollInput() {}

   virtual const InputValues& getInputValues() {
      return INPUT_VALUES;
   }
};

#endif
