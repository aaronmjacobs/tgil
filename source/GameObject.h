#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Types.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class CameraComponent;
class GraphicsComponent;
class InputComponent;
class LightComponent;
class Model;

class GameObject {
protected:
   glm::vec3 position;
   glm::quat orientation;
   glm::vec3 scale;

   SPtr<Model> model;

   // Components
   SPtr<CameraComponent> cameraComponent;
   SPtr<GraphicsComponent> graphicsComponent;
   SPtr<InputComponent> inputComponent;
   SPtr<LightComponent> lightComponent;

   // TODO Make 'null' subclasses for each component which do nothing, and create singletons of each
   // Will allow each component pointer to be guaranteed non-null

public:
   GameObject();

   virtual ~GameObject();

   const glm::vec3& getPosition() const {
      return position;
   }

   const glm::quat& getOrientation() const {
      return orientation;
   }

   const glm::vec3& getScale() const {
      return scale;
   }

   SPtr<Model> getModel() {
      return model;
   }

   void setPosition(const glm::vec3 &position) {
      this->position = position;
   }

   void setOrientation(const glm::quat &orientation) {
      this->orientation = orientation;
   }

   void setScale(const glm::vec3 &scale) {
      this->scale = scale;
   }

   void setModel(SPtr<Model> model) {
      this->model = model;
   }

   CameraComponent& getCameraComponent();
   GraphicsComponent& getGraphicsComponent();
   InputComponent& getInputComponent();
   LightComponent& getLightComponent();

   void setCameraComponent(SPtr<CameraComponent> cameraComponent);
   void setGraphicsComponent(SPtr<GraphicsComponent> graphicsComponent);
   void setInputComponent(SPtr<InputComponent> inputComponent);
   void setLightComponent(SPtr<LightComponent> lightComponent);
};

#endif
