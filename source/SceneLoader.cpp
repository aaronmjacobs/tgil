#include "AssetManager.h"
#include "Context.h"
#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "GhostPhysicsComponent.h"
#include "InputComponent.h"
#include "InputHandler.h"
#include "LogHelper.h"
#include "MeshPhysicsComponent.h"
#include "Model.h"
#include "PhongMaterial.h"
#include "PlayerCameraComponent.h"
#include "PlayerGraphicsComponent.h"
#include "PlayerLogicComponent.h"
#include "PlayerPhysicsComponent.h"
#include "PointLightComponent.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>

#include <sstream>

namespace {

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
   SPtr<Shader> vertexShader = assetManager.loadShader("shaders/phong_vert.glsl", GL_VERTEX_SHADER);
   SPtr<Shader> fragmentShader = assetManager.loadShader("shaders/phong_frag.glsl", GL_FRAGMENT_SHADER);

   SPtr<ShaderProgram> shaderProgram = std::make_shared<ShaderProgram>();
   shaderProgram->attach(vertexShader);
   shaderProgram->attach(fragmentShader);
   bool linked = shaderProgram->link();
   if (!linked) {
      // TODO Handle this in the asset manager?
      LOG_FATAL("Unable to link phong shader");
   }

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

SPtr<PhongMaterial> createPhongMaterial(ShaderProgram &shaderProgram, glm::vec3 color, float specular, float shininess) {
   return std::make_shared<PhongMaterial>(shaderProgram, color * 0.2f, color * 0.6f, glm::vec3(specular), glm::vec3(0.0f), shininess);
}

SPtr<GameObject> createStaticObject(SPtr<Model> model, const glm::vec3 &position, const glm::vec3 &scale, float friction, float restitution) {
   SPtr<GameObject> staticObject(std::make_shared<GameObject>());

   // Transform
   staticObject->setPosition(position);
   staticObject->setScale(scale);

   // Graphics
   staticObject->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*staticObject));
   staticObject->getGraphicsComponent().setModel(model);

   // Physics
   staticObject->setPhysicsComponent(std::make_shared<MeshPhysicsComponent>(*staticObject, 0.0f, CollisionGroup::StaticBodies, CollisionGroup::Everything ^ CollisionGroup::StaticBodies));
   btRigidBody *gameObjectRigidBody = dynamic_cast<btRigidBody*>(staticObject->getPhysicsComponent().getCollisionObject());
   gameObjectRigidBody->setFriction(friction);
   gameObjectRigidBody->setRollingFriction(friction);
   gameObjectRigidBody->setRestitution(restitution);

   return staticObject;
}

SPtr<GameObject> createDynamicObject(SPtr<Model> model, const glm::vec3 &position, const glm::vec3 &scale, float friction, float restitution, float mass) {
   SPtr<GameObject> dynamicObject(std::make_shared<GameObject>());

   // Transform
   dynamicObject->setPosition(position);
   dynamicObject->setScale(scale);

   // Graphics
   dynamicObject->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*dynamicObject));
   dynamicObject->getGraphicsComponent().setModel(model);

   // Physics
   dynamicObject->setPhysicsComponent(std::make_shared<MeshPhysicsComponent>(*dynamicObject, mass, CollisionGroup::Default, CollisionGroup::Everything));
   btRigidBody *gameObjectRigidBody = dynamic_cast<btRigidBody*>(dynamicObject->getPhysicsComponent().getCollisionObject());
   gameObjectRigidBody->setFriction(friction);
   gameObjectRigidBody->setRollingFriction(friction);
   gameObjectRigidBody->setRestitution(restitution);
      
   return dynamicObject;
}

SPtr<GameObject> createPlayer(SPtr<ShaderProgram> shaderProgram, const glm::vec3 &color, SPtr<Mesh> mesh, const glm::vec3 &position, const int playerNum) {
   SPtr<GameObject> player(std::make_shared<GameObject>());

   // Transform
   player->setPosition(position);

   // Graphics
   SPtr<Model> model(std::make_shared<Model>(shaderProgram, createPhongMaterial(*shaderProgram, color, 0.2f, 50.0f), mesh));
   player->setGraphicsComponent(std::make_shared<PlayerGraphicsComponent>(*player));
   player->getGraphicsComponent().setModel(model);

   // Physics
   player->setPhysicsComponent(std::make_shared<PlayerPhysicsComponent>(*player, 1.0f));
   btRigidBody *gameObjectRigidBody = dynamic_cast<btRigidBody*>(player->getPhysicsComponent().getCollisionObject());
   gameObjectRigidBody->setFriction(0.0f);
   gameObjectRigidBody->setRollingFriction(0.0f);
   gameObjectRigidBody->setRestitution(0.0f);

   // Camera
   player->setCameraComponent(std::make_shared<PlayerCameraComponent>(*player));

   // Input
   player->setInputComponent(std::make_shared<InputComponent>(*player, playerNum));

   // Logic
   player->setLogicComponent(std::make_shared<PlayerLogicComponent>(*player, color));
      
   return player;
}

SPtr<GameObject> createLight(const glm::vec3 &position, const glm::vec3 &color, float constFalloff, float linearFalloff, float squareFalloff) {
   SPtr<GameObject> light(std::make_shared<GameObject>());

   // Transform
   light->setPosition(position);

   // Light
   SPtr<PointLightComponent> pointLightComponent(std::make_shared<PointLightComponent>(*light));
   light->setLightComponent(pointLightComponent);
   pointLightComponent->setColor(color);
   pointLightComponent->setConstFalloff(constFalloff);
   pointLightComponent->setLinearFalloff(linearFalloff);
   pointLightComponent->setSquareFalloff(squareFalloff);

   return light;
}

void buildTower(SPtr<Scene> scene, SPtr<Model> model) {
   float height = 20.0f;
   SPtr<GameObject> tower = createStaticObject(model, glm::vec3(0.0f, height / 2.0f, 0.0f), glm::vec3(3.0f, height, 3.0f), 1.0f, 0.3f);
   scene->addObject(tower);

   scene->addObject(createStaticObject(model, glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(3.0f, 1.5f, -3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(0.0f, 3.0f, -3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(-3.0f, 4.5f, -3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(-3.0f, 6.0f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(-3.0f, 7.5f, 3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(0.0f, 9.0f, 3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(3.0f, 10.5f, 3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(3.0f, 12.0f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(3.0f, 13.5f, -3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(0.0f, 15.0f, -3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(-3.0f, 16.5f, -3.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(-3.0f, 18.0f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));

   scene->addObject(createStaticObject(model, glm::vec3(9.5f, -1.25f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(10.0f, -1.0f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(10.5f, -0.75f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(11.0f, -0.5f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(11.5f, -0.25f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));
   scene->addObject(createStaticObject(model, glm::vec3(12.0f, 0.0f, 0.0f), glm::vec3(3.0f), 1.0f, 0.3f));

   SPtr<GameObject> winTrigger = std::make_shared<GameObject>();
   float triggerSize = 1.5f;
   winTrigger->setPosition(tower->getPosition() + glm::vec3(0.0f, height / 2.0f + triggerSize, 0.0f));
   winTrigger->setPhysicsComponent(std::make_shared<GhostPhysicsComponent>(*winTrigger, false, CollisionGroup::Characters, glm::vec3(triggerSize)));
   winTrigger->setTickCallback([](GameObject &gameObject, const float dt) {
      btCollisionObject *collisionObject = gameObject.getPhysicsComponent().getCollisionObject();
      if (!collisionObject) {
         return;
      }
      btGhostObject *ghostObject = dynamic_cast<btGhostObject*>(collisionObject);
      if (!ghostObject) {
         return;
      }
      for (int i = 0; i < ghostObject->getNumOverlappingObjects(); i++) {
         SPtr<Scene> scene = gameObject.getScene().lock();
         if (!scene) {
            return;
         }

         if (!scene->getGameState().hasWinner()) {
            GameObject *collidingGameObject = static_cast<GameObject*>(ghostObject->getOverlappingObject(i)->getUserPointer());

            if (collidingGameObject) {
               int playerNum = collidingGameObject->getInputComponent().getPlayerNum();
               scene->setWinner(playerNum);
            }
         }

         btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(ghostObject->getOverlappingObject(i));
         if(rigidBody) {
            rigidBody->applyCentralForce(btVector3(0.0f, 50.0f, 0.0f));
         }
      }
   });
   scene->addObject(winTrigger);
}

} // namespace

namespace SceneLoader {

SPtr<Scene> loadDefaultScene(const Context &context) {
   SPtr<Scene> scene(std::make_shared<Scene>());
   AssetManager &assetManager = context.getAssetManager();

   SPtr<ShaderProgram> phongShaderProgram = loadPhongShaderProgram(assetManager);

   SPtr<PhongMaterial> boxMaterial = createPhongMaterial(*phongShaderProgram, glm::vec3(0.1f, 0.3f, 0.8f), 0.2f, 50.0f);
   SPtr<PhongMaterial> planeMaterial = createPhongMaterial(*phongShaderProgram, glm::vec3(0.4f), 0.2f, 50.0f);

   SPtr<Mesh> boxMesh = assetManager.loadMesh("meshes/cube.obj");
   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xz_plane.obj");
   SPtr<Mesh> playerMesh = assetManager.loadMesh("meshes/player.obj");

   SPtr<Model> boxModel(std::make_shared<Model>(phongShaderProgram, boxMaterial, boxMesh));
   SPtr<Model> planeModel(std::make_shared<Model>(phongShaderProgram, planeMaterial, planeMesh));

   // Light
   scene->addLight(createLight(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.7f), 0.001f, 0.0005f, 0.0001f));

   // Ground plane
   scene->addObject(createStaticObject(planeModel, glm::vec3(0.0f), glm::vec3(100.0f), 1.0f, 0.3f));

   // Boxes
   scene->addObject(createDynamicObject(boxModel, glm::vec3(-5.0f, 5.0f, -10.0f), glm::vec3(1.0f), 0.3f, 0.8f, 1.0f));

   // Players
   glm::vec3 colors[] = {
      glm::normalize(glm::vec3(1.0f, 0.2f, 0.2f)) * 1.5f,
      glm::normalize(glm::vec3(0.2f, 1.0f, 0.2f)) * 1.5f,
      glm::normalize(glm::vec3(0.9f, 0.9f, 0.2f)) * 1.5f,
      glm::normalize(glm::vec3(0.9f, 0.2f, 0.9f)) * 1.5f
   };
   for (int i = 0; i < context.getInputHandler().getNumDevices(); ++i) {
      int color = i % (sizeof(colors) / sizeof(glm::vec3));
      SPtr<GameObject> player(createPlayer(phongShaderProgram, colors[color], playerMesh, glm::vec3(5.0f * i, 2.0f, 10.0f), i));
      scene->addPlayer(player);
      scene->addCamera(player);
      scene->addObject(player);
   }

   buildTower(scene, boxModel);

   return scene;
}

} // namespace SceneLoader
