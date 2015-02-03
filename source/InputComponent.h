#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "Component.h"
#include "InputHandler.h"

class InputComponent : public Component {
protected:
   const int playerNum;
   InputValues inputValues;

public:
   InputComponent(GameObject &gameObject, const int playerNum);

   virtual ~InputComponent();

   void update();

   const int getPlayerNum() const;

   const InputValues& getInputValues() const;
};

class NullInputComponent : public InputComponent {
public:
   NullInputComponent(GameObject &gameObject)
      : InputComponent(gameObject, -1) {}

   virtual ~NullInputComponent() {}

   virtual void init() {}
};

#endif
