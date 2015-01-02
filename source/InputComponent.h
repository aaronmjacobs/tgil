#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "Component.h"
#include "InputHandler.h"

class Context;
class GameObject;
class NullInputComponent;

class InputComponent : public Component<InputComponent, NullInputComponent> {
public:
   virtual ~InputComponent() {}

   virtual void pollInput(const Context &context, GameObject &gameObject) = 0;

   virtual const InputValues& getInputValues(GameObject &gameObject) = 0;
};

class NullInputComponent : public InputComponent {
private:
   const InputValues INPUT_VALUES;

public:
   NullInputComponent()
      : INPUT_VALUES({ 0 }) {}

   virtual ~NullInputComponent() {}

   virtual void pollInput(const Context &context, GameObject &gameObject) {}

   virtual const InputValues& getInputValues(GameObject &gameObject) {
      return INPUT_VALUES;
   }
};

#endif
