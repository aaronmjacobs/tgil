#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "Component.h"

class GameObject;

class GraphicsComponent : public Component {
public:
   virtual void draw(GameObject &gameObject) = 0;
};

#endif
