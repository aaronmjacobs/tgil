#include "AssetManager.h"
#include "AudioComponent.h"
#include "AudioManager.h"
#include "CameraComponent.h"
#include "Context.h"
#include "Conversions.h"
#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "GhostPhysicsComponent.h"
#include "LightComponent.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "PhongMaterial.h"
#include "PlayerLogicComponent.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "ShoveAbility.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

#include <sstream>

namespace {

const float COOLDOWN = 3.0f;
const float FORCE = 110.0f;
const float LIFE_TIME = 0.25f;
const float SCALE = 2.4f;

} // namespace

ShoveAbility::ShoveAbility(GameObject &gameObject)
   : Ability(gameObject, COOLDOWN) {
}

ShoveAbility::~ShoveAbility() {
}

bool ShoveAbility::use() {
   if (isOnCooldown()) {
      return false;
   }

   SPtr<Scene> scene = gameObject.getScene().lock();
   if (!scene) {
      return false;
   }

   SPtr<GameObject> shove(std::make_shared<GameObject>());

   shove->setPosition(gameObject.getPosition() + gameObject.getCameraComponent().getFrontVector());
   shove->setScale(glm::vec3(SCALE));

   AssetManager &assetManager = Context::getInstance().getAssetManager();

   // Graphics
   SPtr<Mesh> mesh = assetManager.loadMesh("meshes/force_attack.obj");
   glm::vec3 color(1.0f, 0.35f, 0.15f);
   PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&gameObject.getLogicComponent());
   if (playerLogic) {
      color = playerLogic->getColor();
   }
   SPtr<ShaderProgram> shaderProgram(assetManager.loadShaderProgram("shaders/phong"));
   SPtr<Material> material(std::make_shared<PhongMaterial>(color * 0.2f, color * 0.8f, glm::vec3(0.2f), color * 0.2f, 50.0f));
   SPtr<Model> model(std::make_shared<Model>(shaderProgram, mesh));
   model->attachMaterial(material);
   shove->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*shove));
   shove->getGraphicsComponent().setModel(model);
   shove->getGraphicsComponent().setNormalOffsetShadows(false);

   // Light
   shove->setLightComponent(std::make_shared<LightComponent>(*shove, LightComponent::Spot, color * 2.0f, gameObject.getCameraComponent().getFrontVector(), 0.0f, 0.02f, 0.5f, 0.6f));

   // Physics
   shove->setPhysicsComponent(std::make_shared<GhostPhysicsComponent>(*shove, false, CollisionGroup::Default | CollisionGroup::Characters | CollisionGroup::Debries | CollisionGroup::Projectiles));

   // Logic
   const float startTime = Context::getInstance().getRunningTime();
   WPtr<GameObject> wShove(shove);
   GameObject &player = gameObject; // TODO Dangerous - if the "shove" lives longer than the player, this reference will be invalid!
   shove->setTickCallback([&player, startTime, wShove](GameObject &gameObject, const float dt) {
      SPtr<Scene> scene = gameObject.getScene().lock();
      if (!scene) {
         return;
      }

      if (Context::getInstance().getRunningTime() - startTime > LIFE_TIME) {
         SPtr<GameObject> shove(wShove.lock());
         if (shove) {
            scene->removeObject(shove);
            scene->removeLight(shove);
         }
      }

      btCollisionObject *collisionObject = gameObject.getPhysicsComponent().getCollisionObject();
      if (!collisionObject) {
         return;
      }
      btGhostObject *ghostObject = dynamic_cast<btGhostObject*>(collisionObject);
      if (!ghostObject) {
         return;
      }

      const glm::vec3 &cameraPos = player.getCameraComponent().getCameraPosition();
      const glm::vec3 &front = player.getCameraComponent().getFrontVector();

      glm::quat rot = player.getOrientation();
      rot.x *= -1.0f;
      rot.y *= -1.0f;
      rot.z *= -1.0f;
      rot = glm::normalize(rot);

      gameObject.setPosition(cameraPos + front);
      gameObject.setOrientation(rot);
      btTransform &ghostTrans = ghostObject->getWorldTransform();
      ghostTrans.setOrigin(toBt(gameObject.getPosition()));
      ghostTrans.setRotation(toBt(gameObject.getOrientation()));

      gameObject.getLightComponent().setDirection(front);

      glm::vec3 forceDir = rot * glm::vec3(0.0f, 0.0f, -1.0f);
      forceDir.y += 0.5f;

      for (int i = 0; i < ghostObject->getNumOverlappingObjects(); i++) {
         btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(ghostObject->getOverlappingObject(i));
         if(!rigidBody) {
            continue;
         }
         if (rigidBody == player.getPhysicsComponent().getCollisionObject()) {
            continue;
         }

         btVector3 toBody = rigidBody->getWorldTransform().getOrigin() - ghostObject->getWorldTransform().getOrigin();
         float distance = toBody.norm();
         toBody.setY(toBody.y() + 0.25f);
         if (distance == 0.0f) {
            continue;
         }
         btVector3 force = toBt(glm::normalize(forceDir)) * FORCE;

         rigidBody->activate();
         rigidBody->applyCentralForce(force);
      }
   });

   // Audio
   SPtr<AudioComponent> audioComponent(std::make_shared<AudioComponent>(*shove));
   audioComponent->registerSoundEvent(Event::SET_SCENE, SoundGroup::SHOVE);
   shove->setAudioComponent(audioComponent);

   scene->addLight(shove);
   scene->addObject(shove);

   resetTimeSinceLastUse();

   return true;
}
