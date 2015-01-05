#include "CameraComponent.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "LightComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"

GameObject::GameObject()
   : cameraComponent(CameraComponent::getNullComponent()),
     graphicsComponent(GraphicsComponent::getNullComponent()),
     inputComponent(InputComponent::getNullComponent()),
     lightComponent(LightComponent::getNullComponent()),
     physicsComponent(PhysicsComponent::getNullComponent()) {
}

GameObject::~GameObject() {
}

void GameObject::tick(const float dt) {
   physicsComponent->tick(*this);
}

void GameObject::setScene(WPtr<Scene> scene) {
   wScene = scene;
   notify(*this, SET_SCENE);
}

CameraComponent& GameObject::getCameraComponent() {
   return *cameraComponent;
}

void GameObject::setCameraComponent(SPtr<CameraComponent> cameraComponent) {
   this->cameraComponent = cameraComponent;
}

GraphicsComponent& GameObject::getGraphicsComponent() {
   return *graphicsComponent;
}

void GameObject::setGraphicsComponent(SPtr<GraphicsComponent> graphicsComponent) {
   this->graphicsComponent = graphicsComponent;
}

InputComponent& GameObject::getInputComponent() {
   return *inputComponent;
}

void GameObject::setInputComponent(SPtr<InputComponent> inputComponent) {
   this->inputComponent = inputComponent;
}

LightComponent& GameObject::getLightComponent() {
   return *lightComponent;
}

void GameObject::setLightComponent(SPtr<LightComponent> lightComponent) {
   this->lightComponent = lightComponent;
}

PhysicsComponent& GameObject::getPhysicsComponent() {
   return *physicsComponent;
}

void GameObject::setPhysicsComponent(SPtr<PhysicsComponent> physicsComponent) {
   this->physicsComponent = physicsComponent;
   physicsComponent->init(*this);
}
