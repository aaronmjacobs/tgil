#include "CameraComponent.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "LightComponent.h"

GameObject::GameObject() {
}

GameObject::~GameObject() {
}

CameraComponent& GameObject::getCameraComponent() {
   return *cameraComponent;
}

void GameObject::setCameraComponent(UPtr<CameraComponent> cameraComponent) {
   this->cameraComponent = std::move(cameraComponent);
}

GraphicsComponent& GameObject::getGraphicsComponent() {
   return *graphicsComponent;
}

void GameObject::setGraphicsComponent(UPtr<GraphicsComponent> graphicsComponent) {
   this->graphicsComponent = std::move(graphicsComponent);
}

LightComponent& GameObject::getLightComponent() {
   return *lightComponent;
}

void GameObject::setLightComponent(UPtr<LightComponent> lightComponent) {
   this->lightComponent = std::move(lightComponent);
}
