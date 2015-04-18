#ifndef GEOMETRIC_GRAPHICS_COMPONENT_H
#define GEOMETRIC_GRAPHICS_COMPONENT_H

#include "GraphicsComponent.h"

class GeometricGraphicsComponent : public GraphicsComponent {
protected:
   bool castShadows;

public:
   GeometricGraphicsComponent(GameObject &gameObject);

   virtual ~GeometricGraphicsComponent();

   virtual void draw(const RenderData &renderData);

   void enableCastingShadows(bool enabled);
};

#endif
