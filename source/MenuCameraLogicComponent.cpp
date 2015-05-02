#include "AudioManager.h"
#include "CameraComponent.h"
#include "Context.h"
#include "Conversions.h"
#include "GameObject.h"
#include "InputComponent.h"
#include "InputHandler.h"
#include "LogHelper.h"
#include "MenuCameraLogicComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Renderer.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

namespace {

const float INTERP_TIME = 3.0f;
const float MAX_MENU_ITEM_DIST = 5.0f;

void genNearAndFar(int x, int y, int width, int height, const glm::mat4 &view, const glm::mat4 &proj, glm::vec3 &near, glm::vec3 &far) {
   glm::vec3 window(x, height - y, 0);
   glm::vec4 viewport(0, 0, width, height);

   near = glm::unProject(window, view, proj, viewport);
   window.z = 1;
   far = glm::unProject(window, view, proj, viewport);
}

void fireEvent(MenuCameraLogicComponent &menuLogic, const Scene &scene, const btCollisionObject *object, MouseEvent event) {
   const std::vector<ClickableObject> &clickableObjects = scene.getClickableObjecst();
   for (const ClickableObject &clickableObject : clickableObjects) {
      SPtr<GameObject> gameObject = clickableObject.gameObject.lock();
      if (!gameObject) {
         continue; // TODO Remove from vector?
      }

      if (gameObject->getPhysicsComponent().getCollisionObject() == object) {
         clickableObject.clickFunction(menuLogic, event);
      }
   }
}

} // namespace

MenuCameraLogicComponent::MenuCameraLogicComponent(GameObject &gameObject)
: LogicComponent(gameObject), positionInterpTime(0.0f), orientationInterpTime(0.0f), targetPosition(gameObject.getPosition()), lastPosition(targetPosition), targetOrientation(gameObject.getOrientation()), lastOrientation(targetOrientation), lastObject(nullptr), wasClicking(false) {
}

MenuCameraLogicComponent::~MenuCameraLogicComponent() {
}

bool MenuCameraLogicComponent::updatePosition(const float dt) {
   if (targetPosition == lastPosition) {
      return false;
   }

   positionInterpTime += dt;

   glm::vec3 position;
   if (positionInterpTime >= INTERP_TIME) {
      positionInterpTime = 0.0f;
      lastPosition = targetPosition;
      position = targetPosition;
   } else {
      position = glm::lerp(lastPosition, targetPosition, glm::smoothstep(0.0f, INTERP_TIME, positionInterpTime));
   }

   gameObject.setPosition(position);

   return true;
}

bool MenuCameraLogicComponent::updateOrientation(const float dt) {
   if (targetOrientation == lastOrientation) {
      return false;
   }

   orientationInterpTime += dt;

   glm::quat orientation;
   if (orientationInterpTime >= INTERP_TIME) {
      orientationInterpTime = 0.0f;
      lastOrientation = targetOrientation;
      orientation = targetOrientation;
   } else {
      orientation = glm::slerp(lastOrientation, targetOrientation, glm::smoothstep(0.0f, INTERP_TIME, orientationInterpTime));
   }

   gameObject.setOrientation(orientation);

   return true;
}

const btCollisionObject* MenuCameraLogicComponent::getHitObject(InputHandler &inputHandler, const Scene &scene) const {
   const glm::mat4 &view = gameObject.getCameraComponent().getViewMatrix();
   const glm::mat4 &proj = Context::getInstance().getRenderer().getProjectionMatrix();

   glm::vec3 near, far;
   genNearAndFar(inputHandler.getMouseX(), inputHandler.getMouseY(), Context::getInstance().getWindowWidth(), Context::getInstance().getWindowHeight(), view, proj, near, far);

   glm::vec3 dir(glm::normalize(far - near));
   btVector3 from(toBt(near));
   btVector3 to(toBt(near + dir * MAX_MENU_ITEM_DIST));

   btCollisionWorld::ClosestRayResultCallback callback(from, to);
   callback.m_collisionFilterMask = CollisionGroup::Default | CollisionGroup::StaticBodies;
   scene.getPhysicsManager()->getDynamicsWorld().rayTest(from, to, callback);

   return callback.hasHit() ? callback.m_collisionObject : nullptr;
}

void MenuCameraLogicComponent::handleEvents(const Scene &scene, const btCollisionObject *hitObject, bool click) {
   if (lastObject && hitObject != lastObject) {
      fireEvent(*this, scene, lastObject, MouseEvent::Exit);
   }

   if (hitObject && hitObject != lastObject) {
      fireEvent(*this, scene, hitObject, MouseEvent::Enter);
      Context::getInstance().getAudioManager().play(SoundGroup::ENTER);
   }

   if (hitObject && click && !wasClicking) {
      fireEvent(*this, scene, hitObject, MouseEvent::Click);
      Context::getInstance().getAudioManager().play(SoundGroup::CLICK);
   }

   lastObject = hitObject;
   wasClicking = click;
}

void MenuCameraLogicComponent::tick(const float dt) {
   bool moving = updatePosition(dt);

   bool turning = updateOrientation(dt);

   if (moving || turning) {
      return;
   }

   InputHandler &inputHandler = Context::getInstance().getInputHandler();
   SPtr<Scene> scene = gameObject.getScene().lock();
   if (!scene) {
      return;
   }

   const btCollisionObject *hitObject = getHitObject(inputHandler, *scene);
   bool click = inputHandler.isLeftMouseClicked();

   handleEvents(*scene, hitObject, click);
}

void MenuCameraLogicComponent::setTargetPosition(const glm::vec3 &position) {
   lastPosition = gameObject.getPosition();
   targetPosition = position;
   positionInterpTime = 0.0f;
}

void MenuCameraLogicComponent::setTargetOrientation(const glm::quat &orientation) {
   lastOrientation = gameObject.getOrientation();
   targetOrientation = orientation;
   orientationInterpTime = 0.0f;
}
