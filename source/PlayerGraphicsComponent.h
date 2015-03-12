#ifndef PLAYER_GRAPHICS_COMPONENT_H
#define PLAYER_GRAPHICS_COMPONENT_H

#include "GraphicsComponent.h"

class PlayerGraphicsComponent : public GraphicsComponent {
public:
   PlayerGraphicsComponent(GameObject &gameObject);

   virtual ~PlayerGraphicsComponent();

   virtual void draw(const RenderData &renderData);
};


#endif
