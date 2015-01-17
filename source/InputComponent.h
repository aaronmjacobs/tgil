#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "Component.h"
#include "InputHandler.h"

class InputComponent : public Component {
private:
   InputValues inputValues;

protected:
   friend class InputHandler;

   void setInputValues(const InputValues &inputValues) {
      this->inputValues = inputValues;
   }

public:
   InputComponent(GameObject &gameObject)
      : Component(gameObject) {}

   virtual ~InputComponent() {}

   virtual void init() = 0;

   const InputValues& getInputValues() const {
      return inputValues;
   }
};

class NullInputComponent : public InputComponent {
public:
   NullInputComponent(GameObject &gameObject)
      : InputComponent(gameObject) {
      setInputValues({ 0 });
   }

   virtual ~NullInputComponent() {}

   virtual void init() {}
};

#endif
