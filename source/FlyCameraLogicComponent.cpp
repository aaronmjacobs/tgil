#include "CameraComponent.h"
#include "FlyCameraLogicComponent.h"
#include "GameObject.h"
#include "InputComponent.h"

namespace {

const float CAMERA_LOOK_SPEED = 5.0f;
const float CAMERA_MOVE_SPEED = 15.0f;
const float Y_LOOK_BOUND = 0.99f;

} // namespace

FlyCameraLogicComponent::FlyCameraLogicComponent(GameObject &gameObject)
   : LogicComponent(gameObject) {
}

FlyCameraLogicComponent::~FlyCameraLogicComponent() {
}

void FlyCameraLogicComponent::tick(const float dt) {
   const InputValues &inputValues = gameObject.getInputComponent().getInputValues();

   CameraComponent &cameraComponent = gameObject.getCameraComponent();
   float lookAmount = dt * CAMERA_LOOK_SPEED;
   float moveAmount = dt * CAMERA_MOVE_SPEED;

   float pitchAmount = lookAmount * inputValues.lookY;
   float yawAmount = lookAmount * inputValues.lookX;
   glm::vec3 front = cameraComponent.getFrontVector();
   glm::vec3 right = cameraComponent.getRightVector();

   if (front.y - pitchAmount > Y_LOOK_BOUND) {
      pitchAmount = front.y - Y_LOOK_BOUND;
   }
   if (front.y - pitchAmount < -Y_LOOK_BOUND) {
      pitchAmount = front.y + Y_LOOK_BOUND;
   }

   glm::quat pitchChange = glm::angleAxis(pitchAmount, glm::vec3(1.0f, 0.0f, 0.0f));
   glm::quat yawChange = glm::angleAxis(yawAmount, glm::vec3(0.0f, 1.0f, 0.0f));

   glm::vec3 posChange = moveAmount * (((inputValues.moveForward - inputValues.moveBackward) * front) + ((inputValues.moveRight - inputValues.moveLeft) * right));
   gameObject.setPosition(gameObject.getPosition() + posChange);
   gameObject.setOrientation(glm::normalize(pitchChange * gameObject.getOrientation() * yawChange));
}
