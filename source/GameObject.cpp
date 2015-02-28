#include "AudioComponent.h"
#include "CameraComponent.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "LightComponent.h"
#include "LogicComponent.h"
#include "PhysicsComponent.h"

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
   inputComponent->update();
   logicComponent->tick(dt);

   if (tickCallback) {
      tickCallback(*this, dt);
   }
}

void GameObject::setScene(WPtr<Scene> scene) {
   wScene = scene;

   if (!scene.expired()) {
      notify(*this, Event::SET_SCENE);
   }
}

AudioComponent& GameObject::getAudioComponent() const {
   return *audioComponent;
}

void GameObject::setAudioComponent(SPtr<AudioComponent> audioComponent) {
   this->audioComponent = audioComponent;
   addObserver(audioComponent);
}

CameraComponent& GameObject::getCameraComponent() const {
   return *cameraComponent;
}

void GameObject::setCameraComponent(SPtr<CameraComponent> cameraComponent) {
   this->cameraComponent = cameraComponent;
}

GraphicsComponent& GameObject::getGraphicsComponent() const {
   return *graphicsComponent;
}

void GameObject::setGraphicsComponent(SPtr<GraphicsComponent> graphicsComponent) {
   this->graphicsComponent = graphicsComponent;
}

InputComponent& GameObject::getInputComponent() const {
   return *inputComponent;
}

void GameObject::setInputComponent(SPtr<InputComponent> inputComponent) {
   this->inputComponent = inputComponent;
}

LightComponent& GameObject::getLightComponent() const {
   return *lightComponent;
}

void GameObject::setLightComponent(SPtr<LightComponent> lightComponent) {
   this->lightComponent = lightComponent;
}

LogicComponent& GameObject::getLogicComponent() const {
   return *logicComponent;
}

void GameObject::setLogicComponent(SPtr<LogicComponent> logicComponent) {
   this->logicComponent = logicComponent;
}

PhysicsComponent& GameObject::getPhysicsComponent() const {
   return *physicsComponent;
}

void GameObject::setPhysicsComponent(SPtr<PhysicsComponent> physicsComponent) {
   this->physicsComponent = physicsComponent;
   physicsComponent->init();
   addObserver(physicsComponent);
}
