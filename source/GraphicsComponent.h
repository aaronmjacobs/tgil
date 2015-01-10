#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "Component.h"

class GraphicsComponent : public Component {
public:
   GraphicsComponent(GameObject &gameObject)
      : Component(gameObject) {}

   virtual ~GraphicsComponent() {}

   virtual void draw() = 0;
};

class NullGraphicsComponent : public GraphicsComponent {
public:
   NullGraphicsComponent(GameObject &gameObject)
      : GraphicsComponent(gameObject) {}

   virtual ~NullGraphicsComponent() {}

   virtual void draw() {}
};

#endif
