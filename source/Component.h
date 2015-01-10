#ifndef COMPONENT_H
#define COMPONENT_H

#include "Types.h"

class GameObject;

class Component {
protected:
   GameObject &gameObject;

public:
   Component(GameObject &gameObject)
      : gameObject(gameObject) {}

   virtual ~Component() {}
};

#endif
