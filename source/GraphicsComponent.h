#ifndef GRAPHICS_COMPONENT_H
#define GRAPHICS_COMPONENT_H

#include "Component.h"

class NullGraphicsComponent;

class GraphicsComponent : public Component<GraphicsComponent, NullGraphicsComponent> {
public:
   GraphicsComponent(GameObject& gameObject)
      : Component(gameObject) {}

   virtual ~GraphicsComponent() {}

   virtual void draw(GameObject &gameObject) = 0;
};

class NullGraphicsComponent : public GraphicsComponent {
public:
   NullGraphicsComponent()
      : GraphicsComponent(GraphicsComponent::getNullGameObject()) {}

   virtual ~NullGraphicsComponent() {}

   virtual void draw(GameObject &gameObject) {}
};

#endif
