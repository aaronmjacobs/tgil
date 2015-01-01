#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "Component.h"
#include "InputHandler.h"

class Context;
class GameObject;

class InputComponent : public Component {
public:
   virtual ~InputComponent() {}

   virtual void pollInput(const Context &context, GameObject &gameObject) = 0;

   virtual const InputValues& getInputValues(GameObject &gameObject) = 0;
};

#endif
