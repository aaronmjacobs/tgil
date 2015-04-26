#include "AssetManager.h"
#include "AudioComponent.h"
#include "AudioManager.h"
#include "BvhMeshPhysicsComponent.h"
#include "Context.h"
#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "GhostPhysicsComponent.h"
#include "InputComponent.h"
#include "InputHandler.h"
#include "LightComponent.h"
#include "LogHelper.h"
#include "MeshPhysicsComponent.h"
#include "Model.h"
#include "PhongMaterial.h"
#include "PlayerCameraComponent.h"
#include "PlayerGraphicsComponent.h"
#include "PlayerLogicComponent.h"
#include "PlayerPhysicsComponent.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "SunLogicComponent.h"
#include "TextureMaterial.h"
#include "TimeMaterial.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <random>
#include <sstream>

namespace {

SPtr<PhongMaterial> createPhongMaterial(glm::vec3 color, float specular, float shininess, float emission = 0.0f) {
   return std::make_shared<PhongMaterial>(color * 0.3f, color * 0.7f, glm::vec3(specular), color * emission, shininess);
}

SPtr<GameObject> createStaticObject(SPtr<Model> model, const glm::vec3 &position, const glm::vec3 &scale, float friction, float restitution, const glm::quat &orientation = glm::quat()) {
   SPtr<GameObject> staticObject(std::make_shared<GameObject>());

   // Transform
   staticObject->setPosition(position);
   staticObject->setOrientation(orientation);
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

SPtr<GameObject> createBvhObject(SPtr<Model> model, const glm::vec3 &position, const glm::vec3 &scale, float friction, float restitution, const glm::quat orientation = glm::quat()) {
   SPtr<GameObject> staticObject(std::make_shared<GameObject>());

   // Transform
   staticObject->setPosition(position);
   staticObject->setScale(scale);
   staticObject->setOrientation(orientation);

   // Graphics
   staticObject->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*staticObject));
   staticObject->getGraphicsComponent().setModel(model);

   // Physics
   staticObject->setPhysicsComponent(std::make_shared<BvhMeshPhysicsComponent>(*staticObject, CollisionGroup::StaticBodies, CollisionGroup::Everything ^ CollisionGroup::StaticBodies));
   btCollisionObject *collisionObject = staticObject->getPhysicsComponent().getCollisionObject();
   collisionObject->setFriction(friction);
   collisionObject->setRollingFriction(friction);
   collisionObject->setRestitution(restitution);

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
   SPtr<Model> model(std::make_shared<Model>(shaderProgram, mesh));
   model->attachMaterial(createPhongMaterial(color, 0.2f, 50.0f));
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

   // Audio
   SPtr<AudioComponent> audioComponent(std::make_shared<AudioComponent>(*player));
   audioComponent->registerSoundEvent(Event::STEP, SoundGroup::STEP);
   audioComponent->registerSoundEvent(Event::JUMP, SoundGroup::JUMP);
   audioComponent->registerSoundEvent(Event::DIE, SoundGroup::DIE);
   player->setAudioComponent(audioComponent);

   player->setLightComponent(std::make_shared<LightComponent>(*player, LightComponent::Point, color, glm::vec3(0.0f), 0.0f, 0.01f));

   return player;
}

SPtr<GameObject> createPointLight(const glm::vec3 &position, const glm::vec3 &color, float linearFalloff, float squareFalloff) {
   SPtr<GameObject> light(std::make_shared<GameObject>());

   // Transform
   light->setPosition(position);

   // Light
   SPtr<LightComponent> pointLightComponent(std::make_shared<LightComponent>(*light, LightComponent::Point, color, glm::vec3(0.0f), linearFalloff, squareFalloff));
   light->setLightComponent(pointLightComponent);

   return light;
}

SPtr<GameObject> createDirectionalLight(const glm::vec3 &position, const glm::vec3 &color, const glm::vec3 &direction) {
   SPtr<GameObject> light(std::make_shared<GameObject>());

   // Transform
   light->setPosition(position);

   // Light
   SPtr<LightComponent> directionalLightComponent(std::make_shared<LightComponent>(*light, LightComponent::Directional, color, direction));
   light->setLightComponent(directionalLightComponent);

   return light;
}

SPtr<GameObject> createSpotLight(const glm::vec3 &position, const glm::vec3 &color, const glm::vec3 &direction, float linearFalloff, float squareFalloff, float beamAngle, float cutoffAngle) {
   SPtr<GameObject> light(std::make_shared<GameObject>());

   // Transform
   light->setPosition(position);

   // Light
   SPtr<LightComponent> pointLightComponent(std::make_shared<LightComponent>(*light, LightComponent::Spot, color, direction, linearFalloff, squareFalloff, beamAngle, cutoffAngle));
   light->setLightComponent(pointLightComponent);

   return light;
}

void buildDeathVolume(SPtr<Scene> scene, glm::vec3 position, glm::vec3 scale) {
   SPtr<GameObject> volume = std::make_shared<GameObject>();

   volume->setPosition(position);

   volume->setPhysicsComponent(std::make_shared<GhostPhysicsComponent>(*volume, false, CollisionGroup::Characters, scale));

   volume->setTickCallback([](GameObject &gameObject, const float dt) {
      btCollisionObject *collisionObject = gameObject.getPhysicsComponent().getCollisionObject();
      if (!collisionObject) {
         return;
      }
      btGhostObject *ghostObject = dynamic_cast<btGhostObject*>(collisionObject);
      if (!ghostObject) {
         return;
      }
      for (int i = 0; i < ghostObject->getNumOverlappingObjects(); i++) {
         GameObject *collidingGameObject = static_cast<GameObject*>(ghostObject->getOverlappingObject(i)->getUserPointer());
         if (!collidingGameObject) {
            continue;
         }

         PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&collidingGameObject->getLogicComponent());
         if (!playerLogic) {
            continue;
         }

         playerLogic->setAlive(false);
      }
   });

   scene->addObject(volume);
}

const glm::vec3 colors[] = {
   glm::normalize(glm::vec3(1.0f, 0.2f, 0.2f)) * 1.5f,
   glm::normalize(glm::vec3(0.2f, 0.2f, 1.0f)) * 2.0f,
   glm::normalize(glm::vec3(0.9f, 0.9f, 0.2f)) * 1.5f,
   glm::normalize(glm::vec3(0.2f, 1.0f, 0.2f)) * 1.5f
};

SPtr<Scene> loadBasicScene(const Context &context, glm::vec3 spawnLocations[4], std::function<void(Scene &scene)> callback) {
   SPtr<Scene> scene(std::make_shared<Scene>());
   AssetManager &assetManager = context.getAssetManager();

   SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));
   SPtr<ShaderProgram> lavaShaderProgram(assetManager.loadShaderProgram("shaders/lava"));

   GLuint lavatileTextureID = assetManager.loadTexture("textures/lava/lava.jpg", TextureWrap::Repeat);
   GLuint noiseID = assetManager.loadTexture("textures/lava/cloud.png", TextureWrap::Repeat);

   SPtr<TextureMaterial> lavatileMaterial(std::make_shared<TextureMaterial>(lavatileTextureID, "uTexture"));
   SPtr<TextureMaterial> noiseMaterial(std::make_shared<TextureMaterial>(noiseID, "uNoiseTexture"));
   SPtr<TimeMaterial> timeMaterial(std::make_shared<TimeMaterial>());

   SPtr<Mesh> playerMesh = assetManager.loadMesh("meshes/player.obj");
   SPtr<Mesh> lavaMesh = assetManager.loadMesh("meshes/lava.obj");

   SPtr<Model> lavaModel(std::make_shared<Model>(lavaShaderProgram, lavaMesh));
   lavaModel->attachMaterial(lavatileMaterial);
   lavaModel->attachMaterial(noiseMaterial);
   lavaModel->attachMaterial(timeMaterial);

   // Light
   float sunIntensity = 1.25f;
   glm::vec3 sunColor = glm::normalize(glm::vec3(1.0f, 0.95f, 0.75f)) * sunIntensity;
   SPtr<GameObject> sun(createDirectionalLight(glm::vec3(0.0f), sunColor, glm::vec3(0.0f)));
   sun->setLogicComponent(std::make_shared<SunLogicComponent>(*sun));
   scene->addObject(sun);
   scene->addLight(sun);
   scene->setSun(sun);

   //glm::vec3 lavaLightColor = glm::vec3(0.9f, 0.4f, 0.1f) * 0.0f;
   //scene->addLight(createPointLight(glm::vec3(-12.0f, 23.0f, 0.0f), lavaLightColor, 0.0f, 0.0f));
   //scene->addLight(createPointLight(glm::vec3(12.0f, 23.0f, 0.0f), lavaLightColor, 0.0f, 0.0f));
   //scene->addLight(createPointLight(glm::vec3(0.0f, 23.0f, -12.0f), lavaLightColor, 0.0f, 0.0f));
   //scene->addLight(createPointLight(glm::vec3(0.0f, 23.0f, 12.0f), lavaLightColor, 0.0f, 0.0f));

   // Lava
   float deathVolumeSize = 1000.0f;
   float lavaScale = 2.0f;
   scene->addObject(createBvhObject(lavaModel, glm::vec3(0.0f), glm::vec3(lavaScale), 1.0f, 0.3f));
   buildDeathVolume(scene, glm::vec3(0.0f), glm::vec3(deathVolumeSize, 4.0f, deathVolumeSize));

   callback(*scene);

   // Players
   for (int i = 0; i < context.getInputHandler().getNumDevices(); ++i) {
      int color = i % (sizeof(colors) / sizeof(glm::vec3));
      SPtr<GameObject> player(createPlayer(phongShaderProgram, colors[color], playerMesh, spawnLocations[color], i));

      // Make the players look at the center of the map
      glm::vec3 cameraPos = player->getCameraComponent().getCameraPosition();
      glm::vec3 lookAtPos = cameraPos;
      lookAtPos.x = 0.0f;
      lookAtPos.z = 0.0f;

      glm::quat orientation(glm::toQuat(glm::lookAt(cameraPos, lookAtPos, glm::vec3(0.0f, 1.0f, 0.0f))));
      player->setOrientation(orientation);

      scene->addPlayer(player);
      scene->addCamera(player);
      scene->addLight(player);
      scene->addObject(player);
   }

   return scene;
}

} // namespace

namespace SceneLoader {

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
glm::quat randomOrientation() {
   return glm::normalize(glm::angleAxis(1.5f, glm::vec3(distribution(generator), distribution(generator), distribution(generator))));
}

float randomScale(float min, float max) {
   return min + distribution(generator) * max;
}

SPtr<Scene> loadTestScene(const Context &context) {
   glm::vec3 spawnLocations[] = {
      glm::vec3(33.5f, 9.1f, -30.5f),
      glm::vec3(18.2f, 24.7f, 30.0f),
      glm::vec3(-35.2f, 12.6f, 29.6477),
      glm::vec3(-35.7f, 21.5f, -8.18972)
   };

   return loadBasicScene(context, spawnLocations, [&context](Scene &scene) {
      float y = 7.0f;

      AssetManager &assetManager = context.getAssetManager();
      SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

      SPtr<PhongMaterial> floorMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> floorMesh = assetManager.loadMesh("meshes/land.obj");
      SPtr<Model> floorModel(std::make_shared<Model>(phongShaderProgram, floorMesh));
      floorModel->attachMaterial(floorMaterial);
      scene.addObject(createBvhObject(floorModel, glm::vec3(0.0f, y, 0.0f), glm::vec3(1.0f), 1.0f, 0.3f));

      SPtr<PhongMaterial> trunkMaterial = createPhongMaterial(glm::vec3(0.36f, 0.27f, 0.11f), 0.2f, 5.0f);
      SPtr<Mesh> trunkMesh = assetManager.loadMesh("meshes/trunk_lg.obj");
      SPtr<Model> trunkModel(std::make_shared<Model>(phongShaderProgram, trunkMesh));
      trunkModel->attachMaterial(trunkMaterial);

      SPtr<PhongMaterial> leavesMaterial = createPhongMaterial(glm::vec3(0.86f, 0.26f, 0.0f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> leavesMesh = assetManager.loadMesh("meshes/leaves_lg.obj");
      SPtr<Model> leavesModel(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModel->attachMaterial(leavesMaterial);

      SPtr<PhongMaterial> rockMaterial = createPhongMaterial(glm::vec3(0.4f, 0.4f, 0.4f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> rockMesh = assetManager.loadMesh("meshes/rock_lg.obj");
      SPtr<Model> rockModel(std::make_shared<Model>(phongShaderProgram, rockMesh));
      rockModel->attachMaterial(rockMaterial);

      // Trees

      glm::vec3 loc(22.0f, y + 15.6f, 31.0f);
      scene.addObject(createStaticObject(trunkModel, loc, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc2(-37.8f, y + 13.0f, -12.8f);
      scene.addObject(createStaticObject(trunkModel, loc2, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc2, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc3(-31.9f, y + 4.1f, 15.4f);
      scene.addObject(createStaticObject(trunkModel, loc3, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc3, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc4(1.0f, y + -0.2f, -6.8f);
      scene.addObject(createStaticObject(trunkModel, loc4, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc4, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc5(40.3f, y + -1.1f, -32.1f);
      scene.addObject(createStaticObject(trunkModel, loc5, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc5, glm::vec3(1.0f), 1.0f, 0.3f));

      // Rocks

      glm::vec3 loc6(13.0f, y + 5.6f, 10.0f);
      scene.addObject(createStaticObject(rockModel, loc6, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc7(-10.0f, y + 1.0f, 6.0f);
      scene.addObject(createStaticObject(rockModel, loc7, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc8(30.0f, y + 0.5f, -9.0f);
      scene.addObject(createStaticObject(rockModel, loc8, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc9(-33.7f, y + 13.0f, -13.3f);
      scene.addObject(createStaticObject(rockModel, loc9, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc10(-8.0f, y + 2.6f, 33.0f);
      scene.addObject(createStaticObject(rockModel, loc10, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));
   });
}

SPtr<Scene> loadCenterIslandScene(const Context &context) {
   glm::vec3 spawnLocations[] = {
      glm::vec3(-10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, 10.0f),
      glm::vec3(10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, -10.0f)
   };

   return loadBasicScene(context, spawnLocations, [&context](Scene &scene) {
      AssetManager &assetManager = context.getAssetManager();
      SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

      SPtr<PhongMaterial> trunkMaterial = createPhongMaterial(glm::vec3(0.36f, 0.27f, 0.11f), 0.2f, 5.0f);
      SPtr<Mesh> trunkMesh = assetManager.loadMesh("meshes/trunk_lg.obj");
      SPtr<Model> trunkModel(std::make_shared<Model>(phongShaderProgram, trunkMesh));
      trunkModel->attachMaterial(trunkMaterial);

      SPtr<PhongMaterial> leavesMaterial = createPhongMaterial(glm::vec3(0.86f, 0.26f, 0.0f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> leavesMesh = assetManager.loadMesh("meshes/leaves_lg.obj");
      SPtr<Model> leavesModel(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModel->attachMaterial(leavesMaterial);

      SPtr<PhongMaterial> rockMaterial = createPhongMaterial(glm::vec3(0.4f, 0.4f, 0.4f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> rockMesh = assetManager.loadMesh("meshes/rock_lg.obj");
      SPtr<Model> rockModel(std::make_shared<Model>(phongShaderProgram, rockMesh));
      rockModel->attachMaterial(rockMaterial);

      SPtr<PhongMaterial> islandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> islandMesh = assetManager.loadMesh("meshes/island.obj");
      SPtr<Model> islandModel(std::make_shared<Model>(phongShaderProgram, islandMesh));
      islandModel->attachMaterial(islandMaterial);

      SPtr<PhongMaterial> smallIslandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> smallIslandMesh = assetManager.loadMesh("meshes/island_sm.obj");
      SPtr<Model> smallIslandModel(std::make_shared<Model>(phongShaderProgram, smallIslandMesh));
      smallIslandModel->attachMaterial(smallIslandMaterial);

      SPtr<PhongMaterial> bridgeMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> bridgeMesh = assetManager.loadMesh("meshes/bridge.obj");
      SPtr<Model> bridgeModel(std::make_shared<Model>(phongShaderProgram, bridgeMesh));
      bridgeModel->attachMaterial(bridgeMaterial);

      // Island
      scene.addObject(createBvhObject(islandModel, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(5.0f), 1.0f, 0.3f));

      // Trees

      glm::vec3 loc(-11.8f, 15.4f, 9.3f);
      scene.addObject(createStaticObject(trunkModel, loc, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc2(12.3f, 14.7f, -1.9f);
      scene.addObject(createStaticObject(trunkModel, loc2, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc2, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc3(1.0f, 13.7f, -20.1f);
      scene.addObject(createStaticObject(trunkModel, loc3, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc3, glm::vec3(1.0f), 1.0f, 0.3f));

      // Rocks

      glm::vec3 loc6(-4.6f, 14.0f, -9.9f);
      glm::quat ori1(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0.4f, 0.6f, 0.1f))));
      scene.addObject(createStaticObject(rockModel, loc6, glm::vec3(glm::vec3(1.0f)), 1.0f, 0.3f, ori1));

      glm::vec3 loc7(-2.5f, 16.3f, 7.2f);
      glm::quat ori2(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0.7f, 0.3f, 0.8f))));
      scene.addObject(createStaticObject(rockModel, loc7, glm::vec3(glm::vec3(1.0f)), 1.0f, 0.3f, ori2));

      glm::vec3 loc8(11.6f, 15.0f, 12.3f);
      glm::quat ori3(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0.2f, 0.1f, 0.5f))));
      scene.addObject(createStaticObject(rockModel, loc8, glm::vec3(glm::vec3(1.0f)), 1.0f, 0.3f, ori3));

      glm::vec3 loc9(-18.9f, 15.0f, -0.3f);
      glm::quat ori4(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0.9f, 0.4f, 0.7f))));
      scene.addObject(createStaticObject(rockModel, loc9, glm::vec3(glm::vec3(1.0f)), 1.0f, 0.3f, ori4));
   });
}

SPtr<Scene> loadFourBridgedIslandsScene(const Context &context) {
   glm::vec3 spawnLocations[] = {
      glm::vec3(-30.0f, 15.0f, 0.0f),
      glm::vec3(0.0f, 15.0f, 30.0f),
      glm::vec3(30.0f, 15.0f, 0.0f),
      glm::vec3(0.0f, 15.0f, -30.0f)
   };

   return loadBasicScene(context, spawnLocations, [&context](Scene &scene) {
      AssetManager &assetManager = context.getAssetManager();
      SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

      SPtr<PhongMaterial> trunkMaterial = createPhongMaterial(glm::vec3(0.36f, 0.27f, 0.11f), 0.2f, 5.0f);
      SPtr<Mesh> trunkMesh = assetManager.loadMesh("meshes/trunk_lg.obj");
      SPtr<Model> trunkModel(std::make_shared<Model>(phongShaderProgram, trunkMesh));
      trunkModel->attachMaterial(trunkMaterial);

      SPtr<PhongMaterial> leavesMaterial = createPhongMaterial(glm::vec3(0.86f, 0.26f, 0.0f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> leavesMesh = assetManager.loadMesh("meshes/leaves_lg.obj");
      SPtr<Model> leavesModel(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModel->attachMaterial(leavesMaterial);

      SPtr<PhongMaterial> rockMaterial = createPhongMaterial(glm::vec3(0.4f, 0.4f, 0.4f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> rockMesh = assetManager.loadMesh("meshes/rock_lg.obj");
      SPtr<Model> rockModel(std::make_shared<Model>(phongShaderProgram, rockMesh));
      rockModel->attachMaterial(rockMaterial);

      SPtr<PhongMaterial> islandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> islandMesh = assetManager.loadMesh("meshes/island2.obj");
      SPtr<Model> islandModel(std::make_shared<Model>(phongShaderProgram, islandMesh));
      islandModel->attachMaterial(islandMaterial);

      SPtr<PhongMaterial> smallIslandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> smallIslandMesh = assetManager.loadMesh("meshes/island_sm.obj");
      SPtr<Model> smallIslandModel(std::make_shared<Model>(phongShaderProgram, smallIslandMesh));
      smallIslandModel->attachMaterial(smallIslandMaterial);

      SPtr<PhongMaterial> bridgeMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> bridgeMesh = assetManager.loadMesh("meshes/bridge.obj");
      SPtr<Model> bridgeModel(std::make_shared<Model>(phongShaderProgram, bridgeMesh));
      bridgeModel->attachMaterial(bridgeMaterial);

      // Island
      scene.addObject(createBvhObject(islandModel, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.5f), 1.0f, 0.3f));

      // Spawn islands

      scene.addObject(createBvhObject(smallIslandModel, glm::vec3(-30.0f, 10.0f, 0.0f), glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createBvhObject(smallIslandModel, glm::vec3(30.0f, 10.0f, 0.0f), glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createBvhObject(smallIslandModel, glm::vec3(0.0f, 10.0f, -30.0f), glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createBvhObject(smallIslandModel, glm::vec3(0.0f, 10.0f, 30.0f), glm::vec3(1.0f), 1.0f, 0.3f));

      // Bridges
      glm::quat tilt(glm::angleAxis(glm::radians(23.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
      scene.addObject(createBvhObject(bridgeModel, glm::vec3(18.0f, 5.0f, 0.0f), glm::vec3(1.5f), 1.0f, 0.3f, tilt));
      scene.addObject(createBvhObject(bridgeModel, glm::vec3(-18.0f, 5.0f, 0.0f), glm::vec3(1.5f), 1.0f, 0.3f, glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * tilt));
      scene.addObject(createBvhObject(bridgeModel, glm::vec3(0.0f, 5.0f, 18.0f), glm::vec3(1.5f), 1.0f, 0.3f, glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * tilt));
      scene.addObject(createBvhObject(bridgeModel, glm::vec3(0.0f, 5.0f, -18.0f), glm::vec3(1.5f), 1.0f, 0.3f, glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * tilt));

      // Trees

      SPtr<PhongMaterial> leavesMaterialP1 = createPhongMaterial(colors[0], 0.2f, 5.0f);
      SPtr<Model> leavesModelP1(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP1->attachMaterial(leavesMaterialP1);
      glm::vec3 loc(-32.0f, 12.0f, 0.0f);
      scene.addObject(createStaticObject(trunkModel, loc, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP1, loc, glm::vec3(0.75f), 1.0f, 0.3f));

      SPtr<PhongMaterial> leavesMaterialP2 = createPhongMaterial(colors[1], 0.2f, 5.0f);
      SPtr<Model> leavesModelP2(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP2->attachMaterial(leavesMaterialP2);
      glm::vec3 loc2(0.0f, 12.0f, 32.0f);
      scene.addObject(createStaticObject(trunkModel, loc2, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP2, loc2, glm::vec3(0.75f), 1.0f, 0.3f));

      SPtr<PhongMaterial> leavesMaterialP3 = createPhongMaterial(colors[2], 0.2f, 5.0f);
      SPtr<Model> leavesModelP3(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP3->attachMaterial(leavesMaterialP3);
      glm::vec3 loc3(32.0f, 12.0f, 0.0f);
      scene.addObject(createStaticObject(trunkModel, loc3, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP3, loc3, glm::vec3(0.75f), 1.0f, 0.3f));

      SPtr<PhongMaterial> leavesMaterialP4 = createPhongMaterial(colors[3], 0.2f, 5.0f);
      SPtr<Model> leavesModelP4(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP4->attachMaterial(leavesMaterialP4);
      glm::vec3 loc4(0.0f, 12.0f, -32.0f);
      scene.addObject(createStaticObject(trunkModel, loc4, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP4, loc4, glm::vec3(0.75f), 1.0f, 0.3f));

      glm::vec3 loc5(0.0f, 6.0f, 0.0f);
      scene.addObject(createStaticObject(trunkModel, loc5, glm::vec3(1.5f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc5, glm::vec3(1.5f), 1.0f, 0.3f));

      // Rocks

      glm::vec3 loc6(-3.6f, 6.6f, -5.4f);
      glm::quat ori1(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0.4f, 0.6f, 0.1f))));
      scene.addObject(createStaticObject(rockModel, loc6, glm::vec3(glm::vec3(1.0f)), 1.0f, 0.3f, ori1));

      glm::vec3 loc7(-4.5f, 6.3f, 8.2f);
      glm::quat ori2(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0.7f, 0.3f, 0.8f))));
      scene.addObject(createStaticObject(rockModel, loc7, glm::vec3(glm::vec3(1.0f)), 1.0f, 0.3f, ori2));

      glm::vec3 loc8(6.8f, 5.5f, 6.3f);
      glm::quat ori3(glm::normalize(glm::angleAxis(1.5f, glm::vec3(0.2f, 0.1f, 0.5f))));
      scene.addObject(createStaticObject(rockModel, loc8, glm::vec3(glm::vec3(1.0f)), 1.0f, 0.3f, ori3));
   });
}

SPtr<Scene> loadGridIslandScene(const Context &context) {
   const float SIZE = 27.0f;
   const float DISTANCE = 13.0f;
   const float PLATFORM_HEIGHT = 15.0f;
   const float HEIGHT_RANGE = 4.0f;
   const float SPAWN_HEIGHT = PLATFORM_HEIGHT + HEIGHT_RANGE + 5.0f;

   glm::vec3 spawnLocations[] = {
      glm::vec3(-SIZE, SPAWN_HEIGHT, -SIZE),
      glm::vec3(SIZE, SPAWN_HEIGHT, -SIZE),
      glm::vec3(-SIZE, SPAWN_HEIGHT, SIZE),
      glm::vec3(SIZE, SPAWN_HEIGHT, SIZE)
   };

   return loadBasicScene(context, spawnLocations, [&context, SIZE, DISTANCE, PLATFORM_HEIGHT, HEIGHT_RANGE](Scene &scene) {
      AssetManager &assetManager = context.getAssetManager();
      SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

      SPtr<PhongMaterial> trunkMaterial = createPhongMaterial(glm::vec3(0.36f, 0.27f, 0.11f), 0.2f, 5.0f);
      SPtr<Mesh> trunkMesh = assetManager.loadMesh("meshes/trunk_lg.obj");
      SPtr<Model> trunkModel(std::make_shared<Model>(phongShaderProgram, trunkMesh));
      trunkModel->attachMaterial(trunkMaterial);

      SPtr<PhongMaterial> leavesMaterial = createPhongMaterial(glm::vec3(0.86f, 0.26f, 0.0f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> leavesMesh = assetManager.loadMesh("meshes/leaves_lg.obj");
      SPtr<Model> leavesModel(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModel->attachMaterial(leavesMaterial);

      SPtr<PhongMaterial> rockMaterial = createPhongMaterial(glm::vec3(0.4f, 0.4f, 0.4f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> rockMesh = assetManager.loadMesh("meshes/rock_lg.obj");
      SPtr<Model> rockModel(std::make_shared<Model>(phongShaderProgram, rockMesh));
      rockModel->attachMaterial(rockMaterial);

      SPtr<PhongMaterial> islandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> islandMesh = assetManager.loadMesh("meshes/island.obj");
      SPtr<Model> islandModel(std::make_shared<Model>(phongShaderProgram, islandMesh));
      islandModel->attachMaterial(islandMaterial);

      SPtr<PhongMaterial> smallIslandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> smallIslandMesh = assetManager.loadMesh("meshes/island_sm.obj");
      SPtr<Model> smallIslandModel(std::make_shared<Model>(phongShaderProgram, smallIslandMesh));
      smallIslandModel->attachMaterial(smallIslandMaterial);

      SPtr<PhongMaterial> bridgeMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> bridgeMesh = assetManager.loadMesh("meshes/bridge.obj");
      SPtr<Model> bridgeModel(std::make_shared<Model>(phongShaderProgram, bridgeMesh));
      bridgeModel->attachMaterial(bridgeMaterial);

      std::default_random_engine generator;
      std::uniform_real_distribution<float> distribution(-HEIGHT_RANGE, HEIGHT_RANGE);
      std::uniform_real_distribution<float> offsetDistribution(-1.5f, 1.5f);
      std::uniform_real_distribution<float> scaleDistribution(0.7f, 1.3f);
      for (float x = -SIZE; x < SIZE; x += DISTANCE) {
         for (float z = -SIZE; z < SIZE; z += DISTANCE) {
            float heightDiff = distribution(generator);
            glm::vec3 loc(glm::vec3(x, PLATFORM_HEIGHT + heightDiff, z));
            scene.addObject(createBvhObject(smallIslandModel, loc, glm::vec3(1.0f), 1.0f, 0.3f));

            // Don't place anything on the spawn points
            if (glm::abs(x) > SIZE - DISTANCE / 2.0f &&
                glm::abs(z) > SIZE - DISTANCE / 2.0f) {
               continue;
            }

            // Maybe place a tree
            float val = distribution(generator);
            if (val > HEIGHT_RANGE / 4.0f) {
               float treeScale = scaleDistribution(generator);
               glm::vec3 treeLoc(loc + glm::vec3(offsetDistribution(generator), 2.5f, offsetDistribution(generator)));
               scene.addObject(createStaticObject(trunkModel, treeLoc, glm::vec3(treeScale), 1.0f, 0.3f));
               scene.addObject(createStaticObject(leavesModel, treeLoc, glm::vec3(treeScale), 1.0f, 0.3f));
            } // Other 50% of the time, place a rock
            else if (val > 0.0f) {
               glm::quat ori(glm::normalize(glm::angleAxis(1.5f, glm::vec3(offsetDistribution(generator), offsetDistribution(generator), offsetDistribution(generator)))));
               scene.addObject(createStaticObject(rockModel, loc + glm::vec3(offsetDistribution(generator), 3.0f, offsetDistribution(generator)), glm::vec3(glm::vec3(scaleDistribution(generator))), 1.0f, 0.3f, ori));
            }
         }
      }
   });
}

SPtr<Scene> loadTorusIslandScene(const Context &context) {
   const float SIZE = 26.0f;
   const float DISTANCE = 13.0f;
   const float PLATFORM_HEIGHT = 15.0f;
   const float HEIGHT_RANGE = 4.0f;
   const float SPAWN_HEIGHT = PLATFORM_HEIGHT + HEIGHT_RANGE + 5.0f;

   glm::vec3 spawnLocations[] = {
      glm::vec3(-SIZE, SPAWN_HEIGHT, -SIZE),
      glm::vec3(SIZE, SPAWN_HEIGHT, -SIZE),
      glm::vec3(-SIZE, SPAWN_HEIGHT, SIZE),
      glm::vec3(SIZE, SPAWN_HEIGHT, SIZE)
   };

   return loadBasicScene(context, spawnLocations, [&context, SIZE, DISTANCE, PLATFORM_HEIGHT, HEIGHT_RANGE](Scene &scene) {
      AssetManager &assetManager = context.getAssetManager();
      SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

      SPtr<PhongMaterial> trunkMaterial = createPhongMaterial(glm::vec3(0.36f, 0.27f, 0.11f), 0.2f, 5.0f);
      SPtr<Mesh> trunkMesh = assetManager.loadMesh("meshes/trunk_lg.obj");
      SPtr<Model> trunkModel(std::make_shared<Model>(phongShaderProgram, trunkMesh));
      trunkModel->attachMaterial(trunkMaterial);

      SPtr<PhongMaterial> leavesMaterial = createPhongMaterial(glm::vec3(0.86f, 0.26f, 0.0f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> leavesMesh = assetManager.loadMesh("meshes/leaves_lg.obj");
      SPtr<Model> leavesModel(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModel->attachMaterial(leavesMaterial);

      SPtr<PhongMaterial> rockMaterial = createPhongMaterial(glm::vec3(0.4f, 0.4f, 0.4f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> rockMesh = assetManager.loadMesh("meshes/rock_lg.obj");
      SPtr<Model> rockModel(std::make_shared<Model>(phongShaderProgram, rockMesh));
      rockModel->attachMaterial(rockMaterial);

      SPtr<PhongMaterial> torusMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> torusMesh = assetManager.loadMesh("meshes/torus.obj");
      SPtr<Model> torusModel(std::make_shared<Model>(phongShaderProgram, torusMesh));
      torusModel->attachMaterial(torusMaterial);

      SPtr<PhongMaterial> smallIslandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> smallIslandMesh = assetManager.loadMesh("meshes/island_sm.obj");
      SPtr<Model> smallIslandModel(std::make_shared<Model>(phongShaderProgram, smallIslandMesh));
      smallIslandModel->attachMaterial(smallIslandMaterial);

      scene.addObject(createBvhObject(torusModel, glm::vec3(0.0f, 7.0f, 0.0f), glm::vec3(1.8f), 1.0f, 0.3f));
      scene.addObject(createBvhObject(torusModel, glm::vec3(0.0f, 13.0f, 0.0f), glm::vec3(1.0f), 1.0f, 0.3f));

      scene.addObject(createBvhObject(smallIslandModel, glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(2.0f), 1.0f, 0.3f));

      glm::vec3 loc(0.0f, 12.0f, 0.0f);
      scene.addObject(createBvhObject(trunkModel, loc, glm::vec3(5.0f), 1.0f, 0.3f));
      scene.addObject(createBvhObject(leavesModel, loc, glm::vec3(5.0f), 1.0f, 0.3f));
   });
}

SPtr<Scene> loadPlusScene(const Context &context) {
   const float SIZE = 40.0f;
   const float DISTANCE = 13.0f;
   const float PLATFORM_HEIGHT = 15.0f;
   const float HEIGHT_RANGE = 4.0f;
   const float SPAWN_HEIGHT = PLATFORM_HEIGHT + HEIGHT_RANGE + 5.0f;

   glm::vec3 spawnLocations[] = {
      glm::vec3(-SIZE, SPAWN_HEIGHT, 0.0f),
      glm::vec3(SIZE, SPAWN_HEIGHT, 0.0f),
      glm::vec3(0.0f, SPAWN_HEIGHT, -SIZE),
      glm::vec3(0.0f, SPAWN_HEIGHT, SIZE)
   };

   return loadBasicScene(context, spawnLocations, [&context, SIZE, DISTANCE, PLATFORM_HEIGHT, HEIGHT_RANGE](Scene &scene) {
      AssetManager &assetManager = context.getAssetManager();
      SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

      SPtr<PhongMaterial> trunkMaterial = createPhongMaterial(glm::vec3(0.36f, 0.27f, 0.11f), 0.2f, 5.0f);
      SPtr<Mesh> trunkMesh = assetManager.loadMesh("meshes/trunk_lg.obj");
      SPtr<Model> trunkModel(std::make_shared<Model>(phongShaderProgram, trunkMesh));
      trunkModel->attachMaterial(trunkMaterial);

      SPtr<PhongMaterial> leavesMaterial = createPhongMaterial(glm::vec3(0.86f, 0.26f, 0.0f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> leavesMesh = assetManager.loadMesh("meshes/leaves_lg.obj");
      SPtr<Model> leavesModel(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModel->attachMaterial(leavesMaterial);

      SPtr<PhongMaterial> rockMaterial = createPhongMaterial(glm::vec3(0.4f, 0.4f, 0.4f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> rockMesh = assetManager.loadMesh("meshes/rock_lg.obj");
      SPtr<Model> rockModel(std::make_shared<Model>(phongShaderProgram, rockMesh));
      rockModel->attachMaterial(rockMaterial);

      SPtr<PhongMaterial> plusMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> plushMesh = assetManager.loadMesh("meshes/plus.obj");
      SPtr<Model> plusModel(std::make_shared<Model>(phongShaderProgram, plushMesh));
      plusModel->attachMaterial(plusMaterial);

      SPtr<PhongMaterial> smallIslandMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> smallIslandMesh = assetManager.loadMesh("meshes/island_sm.obj");
      SPtr<Model> smallIslandModel(std::make_shared<Model>(phongShaderProgram, smallIslandMesh));
      smallIslandModel->attachMaterial(smallIslandMaterial);

      scene.addObject(createBvhObject(plusModel, glm::vec3(0.0f, 7.0f, 0.0f), glm::vec3(1.0f), 1.0f, 0.3f));

      // Trees

      SPtr<PhongMaterial> leavesMaterialP1 = createPhongMaterial(colors[0], 0.2f, 5.0f);
      SPtr<Model> leavesModelP1(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP1->attachMaterial(leavesMaterialP1);
      glm::vec3 loc(-(SIZE + 4.5f), 13.0f, 0.0f);
      scene.addObject(createStaticObject(trunkModel, loc, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP1, loc, glm::vec3(0.75f), 1.0f, 0.3f));

      SPtr<PhongMaterial> leavesMaterialP2 = createPhongMaterial(colors[1], 0.2f, 5.0f);
      SPtr<Model> leavesModelP2(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP2->attachMaterial(leavesMaterialP2);
      glm::vec3 loc2((SIZE + 4.5f), 13.0f, 0.0f);
      scene.addObject(createStaticObject(trunkModel, loc2, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP2, loc2, glm::vec3(0.75f), 1.0f, 0.3f));

      SPtr<PhongMaterial> leavesMaterialP3 = createPhongMaterial(colors[2], 0.2f, 5.0f);
      SPtr<Model> leavesModelP3(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP3->attachMaterial(leavesMaterialP3);
      glm::vec3 loc3(0.0f, 13.0f, -(SIZE + 4.5f));
      scene.addObject(createStaticObject(trunkModel, loc3, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP3, loc3, glm::vec3(0.75f), 1.0f, 0.3f));

      SPtr<PhongMaterial> leavesMaterialP4 = createPhongMaterial(colors[3], 0.2f, 5.0f);
      SPtr<Model> leavesModelP4(std::make_shared<Model>(phongShaderProgram, leavesMesh));
      leavesModelP4->attachMaterial(leavesMaterialP4);
      glm::vec3 loc4(0.0f, 13.0f, (SIZE + 4.5f));
      scene.addObject(createStaticObject(trunkModel, loc4, glm::vec3(0.75f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModelP4, loc4, glm::vec3(0.75f), 1.0f, 0.3f));

      glm::vec3 loc5(0.0f, 16.0f, 0.0f);
      scene.addObject(createStaticObject(trunkModel, loc5, glm::vec3(1.5f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc5, glm::vec3(1.5f), 1.0f, 0.3f));
   });
}

SPtr<Scene> loadNextScene(const Context &context) {
   static std::vector<std::function<SPtr<Scene> (const Context &context)>> loadFunctions;
   static int index = 0;

   if (loadFunctions.empty()) {
      loadFunctions.push_back(loadTestScene);
      loadFunctions.push_back(loadCenterIslandScene);
      loadFunctions.push_back(loadFourBridgedIslandsScene);
      loadFunctions.push_back(loadGridIslandScene);
      loadFunctions.push_back(loadTorusIslandScene);
      loadFunctions.push_back(loadPlusScene);
   }

   std::function<SPtr<Scene> (const Context &context)> function = loadFunctions[index];
   index = (index + 1) % loadFunctions.size();

   return function(context);
}

} // namespace SceneLoader
