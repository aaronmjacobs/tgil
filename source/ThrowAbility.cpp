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
#include "Mesh.h"
#include "MeshPhysicsComponent.h"
#include "Model.h"
#include "PhongMaterial.h"
#include "PlayerLogicComponent.h"
#include "ProjectileLogicComponent.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "ThrowAbility.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

#include <sstream>

namespace {

const float COOLDOWN = 0.5f;
const float EXPLOSION_FORCE = 65.0f;

SPtr<GameObject> createExplosion(const glm::vec3 &position, const float scale, const glm::vec3 &color) {
   SPtr<GameObject> explosion(std::make_shared<GameObject>());

   explosion->setPosition(position);
   explosion->setScale(glm::vec3(scale));

   AssetManager &assetManager = Context::getInstance().getAssetManager();

   // Graphics
   SPtr<Mesh> sphereMesh = assetManager.loadMesh("meshes/sphere.obj");
   SPtr<ShaderProgram> shaderProgram(assetManager.loadShaderProgram("shaders/phong"));
   SPtr<Material> material(std::make_shared<PhongMaterial>(color * 0.2f, color * 1.0f, glm::vec3(0.2f), glm::vec3(0.0f), 50.0f));
   SPtr<Model> model(std::make_shared<Model>(shaderProgram, sphereMesh));
   model->attachMaterial(material);
   explosion->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*explosion));
   explosion->getGraphicsComponent().setModel(model);

   // Light
   explosion->setLightComponent(std::make_shared<LightComponent>(*explosion, LightComponent::Point, color * 2.0f, glm::vec3(0.0f), 0.0f, 0.1f));

   // Physics
   explosion->setPhysicsComponent(std::make_shared<GhostPhysicsComponent>(*explosion, false, CollisionGroup::Default | CollisionGroup::Characters | CollisionGroup::Debries | CollisionGroup::Projectiles));

   // Logic
   const float startTime = glfwGetTime();
   WPtr<GameObject> wExplosion(explosion);
   explosion->setTickCallback([startTime, wExplosion](GameObject &gameObject, const float dt) {
      SPtr<Scene> scene = gameObject.getScene().lock();
      if (!scene) {
         return;
      }

      if (glfwGetTime() - startTime > 0.1f) {
         SPtr<GameObject> explosion(wExplosion.lock());
         if (explosion) {
            scene->removeObject(explosion);
            scene->removeLight(explosion);
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
      for (int i = 0; i < ghostObject->getNumOverlappingObjects(); i++) {
         btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(ghostObject->getOverlappingObject(i));
         if(!rigidBody) {
            return;
         }

         btVector3 toBody = rigidBody->getWorldTransform().getOrigin() - ghostObject->getWorldTransform().getOrigin();
         btVector3 center;
         float radius;
         ghostObject->getCollisionShape()->getBoundingSphere(center, radius);
         if (radius == 0.0f) {
            radius = 1.0f;
         }
         float distance = toBody.norm();
         float forceScale = glm::pow(glm::max(0.0f, (radius - distance / radius)), 2.0f);
         btVector3 force = toBody.normalized() * EXPLOSION_FORCE * forceScale;

         rigidBody->activate();
         rigidBody->applyCentralForce(force);
      }
   });

   // Audio
   SPtr<AudioComponent> audioComponent(std::make_shared<AudioComponent>(*explosion));
   audioComponent->registerSoundEvent(Event::SET_SCENE, SoundGroup::EXPLOSION);
   explosion->setAudioComponent(audioComponent);

   return explosion;
}

} // namespace

ThrowAbility::ThrowAbility(GameObject &gameObject)
   : Ability(gameObject, COOLDOWN) {
}

ThrowAbility::~ThrowAbility() {
}

void ThrowAbility::use() {
   if (isOnCooldown()) {
      return;
   }

   SPtr<Scene> scene = gameObject.getScene().lock();
   if (!scene) {
      return;
   }

   const glm::vec3 &position = gameObject.getCameraComponent().getCameraPosition();
   const glm::vec3 &front = gameObject.getCameraComponent().getFrontVector();
   const glm::vec3 &right = gameObject.getCameraComponent().getRightVector();

   SPtr<GameObject> projectile(std::make_shared<GameObject>());
   projectile->setPosition(position + front + right * 0.25f);
   projectile->setScale(glm::vec3(0.2f));

   // Graphics
   SPtr<Model> playerModel = gameObject.getGraphicsComponent().getModel();
   SPtr<Mesh> mesh = Context::getInstance().getAssetManager().loadMesh("meshes/sphere.obj");
   glm::vec3 color(1.0f, 0.75f, 0.15f);
   PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&gameObject.getLogicComponent());
   if (playerLogic) {
      color = playerLogic->getColor();
   }
   SPtr<Material> material(std::make_shared<PhongMaterial>(color * 0.2f, color * 0.6f, glm::vec3(0.2f), glm::vec3(0.0f), 50.0f));
   SPtr<Model> model(std::make_shared<Model>(playerModel->getShaderProgram(), mesh));
   model->attachMaterial(material);
   projectile->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*projectile));
   projectile->getGraphicsComponent().setModel(model);

   // Physics
   projectile->setPhysicsComponent(std::make_shared<MeshPhysicsComponent>(*projectile, 0.05f, CollisionGroup::Projectiles, CollisionGroup::Everything));
   btRigidBody *projectileRigidBody = dynamic_cast<btRigidBody*>(projectile->getPhysicsComponent().getCollisionObject());
   projectileRigidBody->setFriction(1.0f);
   projectileRigidBody->setRollingFriction(0.25f);
   projectileRigidBody->setRestitution(0.5f);
   projectileRigidBody->applyCentralForce(toBt(front * 100.0f));

   // Logic
   SPtr<ProjectileLogicComponent> logic(std::make_shared<ProjectileLogicComponent>(*projectile));
   WPtr<GameObject> wProjectile(projectile);
   logic->setCollisionCallback([wProjectile, color](GameObject &gameObject, const float lifeTime, const float dt) {
      if (lifeTime < 0.25f) {
         //return;
      }

      SPtr<Scene> scene = gameObject.getScene().lock();
      if (!scene) {
         return;
      }

      SPtr<GameObject> projectile = wProjectile.lock();
      if (!projectile) {
         return;
      }

      scene->removeObject(projectile);

      SPtr<GameObject> explosion(createExplosion(projectile->getPosition(), 1.0f, color));
      scene->addLight(explosion);
      scene->addObject(explosion);
   });
   projectile->setLogicComponent(logic);

   // Audio
   SPtr<AudioComponent> audioComponent(std::make_shared<AudioComponent>(*projectile));
   audioComponent->registerSoundEvent(Event::SET_SCENE, SoundGroup::THROW);
   projectile->setAudioComponent(audioComponent);

   scene->addObject(projectile);

   resetTimeSinceLastUse();
}
