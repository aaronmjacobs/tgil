#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Subject.h"
#include "Transform.h"
#include "Types.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <functional>

class CameraComponent;
class GraphicsComponent;
class InputComponent;
class LightComponent;
class LogicComponent;
class PhysicsComponent;
class Scene;

class GameObject : public Subject<GameObject> {
protected:
   // Position and orientation
   Transform transform;

   // The scene that the object resides in
   WPtr<Scene> wScene;

   // Components
   SPtr<CameraComponent> cameraComponent;
   SPtr<GraphicsComponent> graphicsComponent;
   SPtr<InputComponent> inputComponent;
   SPtr<LightComponent> lightComponent;
   SPtr<LogicComponent> logicComponent;
   SPtr<PhysicsComponent> physicsComponent;

   // Tick callback
   std::function<void(GameObject&, const float dt)> tickCallback;

public:
   GameObject();

   virtual ~GameObject();

   virtual void tick(const float dt);

   const glm::vec3& getPosition() const {
      return transform.position;
   }

   const glm::quat& getOrientation() const {
      return transform.orientation;
   }

   const glm::vec3& getScale() const {
      return transform.scale;
   }

   void setScene(WPtr<Scene> scene);

   WPtr<Scene> getScene() const {
      return wScene;
   }

   void setPosition(const glm::vec3 &position) {
      transform.position = position;
   }

   void setOrientation(const glm::quat &orientation) {
      transform.orientation = orientation;
   }

   void setScale(const glm::vec3 &scale) {
      transform.scale = scale;
      notify(*this, Event::SCALE);
   }

   void setTickCallback(std::function<void(GameObject&, const float dt)> tickCallback) {
      this->tickCallback = tickCallback;
   }

   void clearTickCallback() {
      tickCallback = nullptr;
   }

   CameraComponent& getCameraComponent() const;
   GraphicsComponent& getGraphicsComponent() const;
   InputComponent& getInputComponent() const;
   LightComponent& getLightComponent() const;
   LogicComponent& getLogicComponent() const;
   PhysicsComponent& getPhysicsComponent() const;

   void setCameraComponent(SPtr<CameraComponent> cameraComponent);
   void setGraphicsComponent(SPtr<GraphicsComponent> graphicsComponent);
   void setInputComponent(SPtr<InputComponent> inputComponent);
   void setLightComponent(SPtr<LightComponent> lightComponent);
   void setLogicComponent(SPtr<LogicComponent> logicComponent);
   void setPhysicsComponent(SPtr<PhysicsComponent> physicsComponent);
};

#endif
