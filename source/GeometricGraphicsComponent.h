#ifndef GEOMETRIC_GRAPHICS_COMPONENT_H
#define GEOMETRIC_GRAPHICS_COMPONENT_H

#include "GraphicsComponent.h"

class GeometricGraphicsComponent : public GraphicsComponent {
public:
   virtual void draw(GameObject &gameObject);
};

#endif
