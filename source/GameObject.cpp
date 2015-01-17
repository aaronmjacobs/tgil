#include "CameraComponent.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "LightComponent.h"
#include "LogicComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"

GameObject::GameObject()
   : cameraComponent(std::make_shared<NullCameraComponent>(*this)),
     graphicsComponent(std::make_shared<NullGraphicsComponent>(*this)),
     inputComponent(std::make_shared<NullInputComponent>(*this)),
     lightComponent(std::make_shared<NullLightComponent>(*this)),
     logicComponent(std::make_shared<NullLogicComponent>(*this)),
     physicsComponent(std::make_shared<NullPhysicsComponent>(*this)) {
}

GameObject::~GameObject() {
}

void GameObject::tick(const float dt) {
   logicComponent->tick(dt);

   if (tickCallback) {
      tickCallback(*this, dt);
   }
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
   inputComponent->init();
}

LightComponent& GameObject::getLightComponent() {
   return *lightComponent;
}

void GameObject::setLightComponent(SPtr<LightComponent> lightComponent) {
   this->lightComponent = lightComponent;
}

LogicComponent& GameObject::getLogicComponent() {
   return *logicComponent;
}

void GameObject::setLogicComponent(SPtr<LogicComponent> logicComponent) {
   this->logicComponent = logicComponent;
}

PhysicsComponent& GameObject::getPhysicsComponent() {
   return *physicsComponent;
}

void GameObject::setPhysicsComponent(SPtr<PhysicsComponent> physicsComponent) {
   this->physicsComponent = physicsComponent;
   physicsComponent->init();
}
