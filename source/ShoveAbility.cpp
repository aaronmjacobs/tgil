#include "AssetManager.h"
#include "CameraComponent.h"
#include "Context.h"
#include "Conversions.h"
#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "GhostPhysicsComponent.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "PhongMaterial.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "ShoveAbility.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

#include <sstream>

namespace {

const float COOLDOWN = 5.0f;
const float FORCE = 75.0f;
const float LIFE_TIME = 0.25f;

// TODO Handle in asset manager somehow
const int MAX_LIGHTS = 10;

void addLightUniforms(ShaderProgram &shaderProgram) {
   for (int i = 0; i < MAX_LIGHTS; ++i) {
      std::stringstream ss;
      ss << "uLights[" << i << "].";
      std::string lightName = ss.str();

      shaderProgram.addUniform(lightName + "position");
      shaderProgram.addUniform(lightName + "color");
      shaderProgram.addUniform(lightName + "constFalloff");
      shaderProgram.addUniform(lightName + "linearFalloff");
      shaderProgram.addUniform(lightName + "squareFalloff");
   }
}

SPtr<ShaderProgram> loadPhongShaderProgram(AssetManager &assetManager) {
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/phong");

   shaderProgram->addUniform("uProjMatrix");
   shaderProgram->addUniform("uViewMatrix");
   shaderProgram->addUniform("uModelMatrix");
   shaderProgram->addUniform("uNormalMatrix");
   shaderProgram->addUniform("uNumLights");
   shaderProgram->addUniform("uMaterial.ambient");
   shaderProgram->addUniform("uMaterial.diffuse");
   shaderProgram->addUniform("uMaterial.emission");
   shaderProgram->addUniform("uMaterial.shininess");
   shaderProgram->addUniform("uMaterial.specular");
   shaderProgram->addUniform("uCameraPos");
   addLightUniforms(*shaderProgram);

   shaderProgram->addAttribute("aPosition");
   shaderProgram->addAttribute("aNormal");
      
   return shaderProgram;
}

} // namespace

ShoveAbility::ShoveAbility(GameObject &gameObject)
   : Ability(gameObject, COOLDOWN) {
}

ShoveAbility::~ShoveAbility() {
}
#include "LogHelper.h"
void ShoveAbility::use() {
   if (isOnCooldown()) {
      return;
   }

   SPtr<Scene> scene = gameObject.getScene().lock();
   if (!scene) {
      return;
   }

   SPtr<GameObject> shove(std::make_shared<GameObject>());

   shove->setPosition(gameObject.getPosition());
   shove->setScale(glm::vec3(1.0f));

   AssetManager &assetManager = Context::getInstance().getAssetManager();

   // Graphics
   SPtr<Mesh> mesh = assetManager.loadMesh("meshes/cone.obj");
   glm::vec3 color(1.0f, 0.35f, 0.15f);
   SPtr<ShaderProgram> shaderProgram(loadPhongShaderProgram(assetManager));
   SPtr<Material> material(std::make_shared<PhongMaterial>(*shaderProgram, color * 0.2f, color * 0.6f, glm::vec3(0.2f), glm::vec3(0.0f), 50.0f));
   SPtr<Model> model(std::make_shared<Model>(shaderProgram, material, mesh));
   shove->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*shove));
   shove->getGraphicsComponent().setModel(model);

   // Physics
   shove->setPhysicsComponent(std::make_shared<GhostPhysicsComponent>(*shove, false, CollisionGroup::Default | CollisionGroup::Characters | CollisionGroup::Debries));

   // Logic
   const float startTime = glfwGetTime();
   WPtr<GameObject> wShove(shove);
   GameObject &player = gameObject; // TODO Dangerous - if the "shove" lives longer than the player, this reference will be invalid!
   shove->setTickCallback([&player, startTime, wShove](GameObject &gameObject, const float dt) {
      SPtr<Scene> scene = gameObject.getScene().lock();
      if (!scene) {
         return;
      }

      if (glfwGetTime() - startTime > LIFE_TIME) {
         SPtr<GameObject> shove(wShove.lock());
         if (shove) {
            scene->removeObject(shove);
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

      glm::quat rot = player.getOrientation();
      rot.x *= -1.0f;
      rot.y *= -1.0f;
      rot.z *= -1.0f;
      rot = glm::normalize(rot);

      gameObject.setPosition(cameraPos);
      gameObject.setOrientation(rot);
      btTransform &ghostTrans = ghostObject->getWorldTransform();
      ghostTrans.setOrigin(toBt(cameraPos));
      ghostTrans.setRotation(toBt(rot));

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

   scene->addObject(shove);

   resetTimeSinceLastUse();
}
