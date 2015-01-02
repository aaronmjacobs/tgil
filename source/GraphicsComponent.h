#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "Component.h"

class GameObject;
class NullGraphicsComponent;

class GraphicsComponent : public Component<GraphicsComponent, NullGraphicsComponent> {
public:
   virtual ~GraphicsComponent() {}

   virtual void draw(GameObject &gameObject) = 0;
};

class NullGraphicsComponent : public GraphicsComponent {
public:
   virtual ~NullGraphicsComponent() {}

   virtual void draw(GameObject &gameObject) {}
};

#endif
