#include "GameObject.h"
#include "GraphicsComponent.h"

void GameObject::setGraphicsComponent(UPtr<GraphicsComponent> graphicsComponent) {
   this->graphicsComponent = std::move(graphicsComponent);
}
