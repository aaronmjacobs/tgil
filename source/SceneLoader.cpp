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

SPtr<GameObject> createBvhObject(SPtr<Model> model, const glm::vec3 &position, const glm::vec3 &scale, float friction, float restitution) {
   SPtr<GameObject> staticObject(std::make_shared<GameObject>());

   // Transform
   staticObject->setPosition(position);
   staticObject->setScale(scale);

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
   player->setAudioComponent(audioComponent);

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

void buildTower(Scene &scene, SPtr<Model> boxModel, glm::vec3 bottom, float stepHeight, float boxScale, float radius, int numBoxes) {
   int state = 0;
   float x = -radius, z = -radius;
   for (int i = 0; i < numBoxes; ++i) {
      float y = i * stepHeight;

      if (state == 0 && x >= radius) {
         state = 1;
         z += boxScale - (stepHeight * 2.0f);
      }
      if (state == 1 && z >= radius) {
         state = 2;
         x -= boxScale - (stepHeight * 2.0f);
      }
      if (state == 2 && x <= -radius) {
         state = 3;
         z -= boxScale - (stepHeight * 2.0f);
      }
      if (state == 3 && z <= -radius) {
         state = 0;
         x += boxScale - (stepHeight * 2.0f);
      }

      if (state == 0) {
         x += stepHeight * 2.0f;
      } else if (state == 1) {
         z += stepHeight * 2.0f;
      } else if (state == 2) {
         x -= stepHeight * 2.0f;
      } else {
         z -= stepHeight * 2.0f;
      }

      scene.addObject(createStaticObject(boxModel, glm::vec3(x, y, z) + bottom, glm::vec3(boxScale), 1.0f, 0.3f));
   }
}

const glm::vec3 colors[] = {
   glm::normalize(glm::vec3(1.0f, 0.2f, 0.2f)) * 1.5f,
   glm::normalize(glm::vec3(0.2f, 0.2f, 1.0f)) * 1.5f,
   glm::normalize(glm::vec3(0.9f, 0.9f, 0.2f)) * 1.5f,
   glm::normalize(glm::vec3(0.2f, 1.0f, 0.2f)) * 1.5f
};

SPtr<Scene> loadBasicScene(const Context &context, glm::vec3 spawnLocations[4], std::function<void(Scene &scene, SPtr<Model> boxModel)> callback) {
   SPtr<Scene> scene(std::make_shared<Scene>());
   AssetManager &assetManager = context.getAssetManager();

   SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));
   SPtr<ShaderProgram> phongTextureShaderProgram(assetManager.loadShaderProgram("shaders/phong_textured"));
   SPtr<ShaderProgram> tilingTextureShaderProgram(assetManager.loadShaderProgram("shaders/tiling_texture"));
   SPtr<ShaderProgram> lavaShaderProgram(assetManager.loadShaderProgram("shaders/lava"));
   SPtr<ShaderProgram> skyboxShaderProgram(assetManager.loadShaderProgram("shaders/skybox"));

   //GLuint lavaTextureID = assetManager.loadTexture("textures/lava.png", TextureWrap::Repeat);
   GLuint rockTextureID = assetManager.loadTexture("textures/soil.jpg", TextureWrap::Repeat);
   GLuint skyboxID = assetManager.loadCubemap("textures/desert", "jpg");

   GLuint lavatileTextureID = assetManager.loadTexture("textures/lavatile2.jpg", TextureWrap::Repeat);
   GLuint noiseID = assetManager.loadTexture("textures/cloud.png", TextureWrap::Repeat);

   SPtr<PhongMaterial> boxMaterial = createPhongMaterial(glm::vec3(1.0f), 0.2f, 50.0f);
   SPtr<PhongMaterial> lavaPhongMaterial = createPhongMaterial(glm::vec3(0.97f, 0.21f, 0.08f), 0.2f, 5.0f, 0.2f);
   //SPtr<PhongMaterial> planeMaterial = createPhongMaterial(*phongShaderProgram, glm::vec3(0.4f), 0.2f, 50.0f);
   SPtr<TextureMaterial> rockMaterial(std::make_shared<TextureMaterial>(rockTextureID, "uTexture"));
   //SPtr<TextureMaterial> lavaMaterial(std::make_shared<TextureMaterial>(*tilingTextureShaderProgram, lavaTextureID, "uTexture"));
   SPtr<TextureMaterial> skyboxMaterial(std::make_shared<TextureMaterial>(skyboxID, "uSkybox", GL_TEXTURE_CUBE_MAP));
   SPtr<TextureMaterial> lavatileMaterial(std::make_shared<TextureMaterial>(lavatileTextureID, "uTexture"));
   SPtr<TextureMaterial> noiseMaterial(std::make_shared<TextureMaterial>(noiseID, "uNoiseTexture"));
   SPtr<TimeMaterial> timeMaterial(std::make_shared<TimeMaterial>());

   SPtr<Mesh> skyboxMesh = assetManager.loadMesh("meshes/skybox.obj");
   SPtr<Mesh> boxMesh = assetManager.loadMesh("meshes/cube.obj");
   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xz_plane.obj");
   SPtr<Mesh> playerMesh = assetManager.loadMesh("meshes/player.obj");
   SPtr<Mesh> lavaMesh = assetManager.loadMesh("meshes/lava.obj");

   SPtr<Model> skyboxModel(std::make_shared<Model>(skyboxShaderProgram, skyboxMesh));
   skyboxModel->attachMaterial(skyboxMaterial);
   SPtr<Model> boxModel(std::make_shared<Model>(phongTextureShaderProgram, boxMesh));
   boxModel->attachMaterial(rockMaterial);
   boxModel->attachMaterial(boxMaterial);
   //SPtr<Model> planeModel(std::make_shared<Model>(phongShaderProgram, planeMaterial, planeMesh));
   SPtr<Model> lavaModel(std::make_shared<Model>(phongShaderProgram, lavaMesh));
   lavaModel->attachMaterial(lavaPhongMaterial);
   //lavaModel->attachMaterial(lavatileMaterial);
   //lavaModel->attachMaterial(noiseMaterial);
   //lavaModel->attachMaterial(timeMaterial);
   //SPtr<Model> lavalModel(std::make_shared<Model>(tilingTextureShaderProgram, lavaMaterial, planeMesh));

   SPtr<GameObject> skybox(std::make_shared<GameObject>());
   skybox->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*skybox));
   skybox->getGraphicsComponent().setModel(skyboxModel);
   scene->setSkybox(skybox);

   // Light
   float sunDistance = 300.0f;
   glm::vec3 sunDir(1.0f, -1.0f, -0.5f);
   glm::vec3 sunPos = -sunDir * sunDistance;
   float sunIntensity = 1.25f;
   glm::vec3 sunColor = glm::normalize(glm::vec3(1.0f, 0.95f, 0.75f)) * sunIntensity;
   scene->addLight(createDirectionalLight(sunPos, sunColor, sunDir));

   //scene->addLight(createSpotLight(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f, 0.4f, 0.5f));

   //glm::vec3 lavaLightColor = glm::vec3(0.9f, 0.4f, 0.1f) * 0.0f;
   //scene->addLight(createPointLight(glm::vec3(-12.0f, 23.0f, 0.0f), lavaLightColor, 0.0f, 0.0f));
   //scene->addLight(createPointLight(glm::vec3(12.0f, 23.0f, 0.0f), lavaLightColor, 0.0f, 0.0f));
   //scene->addLight(createPointLight(glm::vec3(0.0f, 23.0f, -12.0f), lavaLightColor, 0.0f, 0.0f));
   //scene->addLight(createPointLight(glm::vec3(0.0f, 23.0f, 12.0f), lavaLightColor, 0.0f, 0.0f));

   // Lava
   float lavaSize = 100.0f;
   scene->addObject(createBvhObject(lavaModel, glm::vec3(0.0f), glm::vec3(1.0f), 1.0f, 0.3f));
   buildDeathVolume(scene, glm::vec3(0.0f), glm::vec3(lavaSize, 3.0f, lavaSize));

   callback(*scene, boxModel);

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
      scene->addObject(player);
   }

   return scene;
}

} // namespace

namespace SceneLoader {

SPtr<Scene> loadTowerScene(const Context &context) {
   SPtr<Scene> scene(std::make_shared<Scene>());
   AssetManager &assetManager = context.getAssetManager();

   SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

   SPtr<PhongMaterial> boxMaterial = createPhongMaterial(glm::vec3(0.1f, 0.3f, 0.8f), 0.2f, 50.0f);
   SPtr<PhongMaterial> planeMaterial = createPhongMaterial(glm::vec3(0.4f), 0.2f, 50.0f);

   SPtr<Mesh> boxMesh = assetManager.loadMesh("meshes/cube.obj");
   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xz_plane.obj");
   SPtr<Mesh> playerMesh = assetManager.loadMesh("meshes/player.obj");

   SPtr<Model> boxModel(std::make_shared<Model>(phongShaderProgram, boxMesh));
   boxModel->attachMaterial(boxMaterial);
   SPtr<Model> planeModel(std::make_shared<Model>(phongShaderProgram, planeMesh));
   planeModel->attachMaterial(planeMaterial);

   // Light
   scene->addLight(createPointLight(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.7f), 0.0005f, 0.0001f));

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

SPtr<Scene> loadCenterPlatformScene(const Context &context) {
   glm::vec3 spawnLocations[] = {
      glm::vec3(-10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, 10.0f),
      glm::vec3(10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, -10.0f)
   };

   return loadBasicScene(context, spawnLocations, [&context](Scene &scene, SPtr<Model> boxModel) {
      // Single platform
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f), glm::vec3(30.0f, 20.0f, 30.0f), 1.0f, 0.3f));

      SPtr<GameObject> spotLight(std::make_shared<GameObject>());
      spotLight->setPosition(glm::vec3(4.0f, 20.0f, 5.0f));
      spotLight->setLightComponent(std::make_shared<LightComponent>(*spotLight, LightComponent::Spot, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(-0.4f, -0.9f, -0.7f), 0.0f, 0.02f, 0.5f, 0.6f));
      scene.addLight(spotLight);
   });
}

SPtr<Scene> loadMiniTowersScene(const Context &context) {
   const float distanceFromCenter = 50.0f;

   glm::vec3 spawnLocations[] = {
      glm::vec3(-distanceFromCenter, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, distanceFromCenter),
      glm::vec3(distanceFromCenter, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, -distanceFromCenter)
   };

   return loadBasicScene(context, spawnLocations, [distanceFromCenter](Scene &scene, SPtr<Model> boxModel) {
      // Center platform
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f), glm::vec3(15.0f, 20.0f, 15.0f), 1.0f, 0.3f));

      // Center tower
      buildTower(scene, boxModel, glm::vec3(0.0f, 9.0f, 0.0f), 0.4f, 2.0f, 3.5f, 43);
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f), glm::vec3(5.0f, 54.0f, 5.0f), 1.0f, 0.3f));

      // Four outer platforms
      scene.addObject(createStaticObject(boxModel, glm::vec3(-distanceFromCenter, 0.0f, 0.0f), glm::vec3(5.0f, 20.0f, 5.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 0.0f, distanceFromCenter), glm::vec3(5.0f, 20.0f, 5.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(distanceFromCenter, 0.0f, 0.0f), glm::vec3(5.0f, 20.0f, 5.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 0.0f, -distanceFromCenter), glm::vec3(5.0f, 20.0f, 5.0f), 1.0f, 0.3f));

      // Four tightropes
      const float TIGHTROPE_WIDTH = 2.0f;
      scene.addObject(createStaticObject(boxModel, glm::vec3(-distanceFromCenter / 2.0f, 0.0f, 0.0f), glm::vec3(distanceFromCenter, 18.0f, TIGHTROPE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 0.0f, distanceFromCenter / 2.0f), glm::vec3(TIGHTROPE_WIDTH, 18.0f, distanceFromCenter), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(distanceFromCenter / 2.0f, 0.0f, 0.0f), glm::vec3(distanceFromCenter, 18.0f, TIGHTROPE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 0.0f, -distanceFromCenter / 2.0f), glm::vec3(TIGHTROPE_WIDTH, 18.0f, distanceFromCenter), 1.0f, 0.3f));
   });
}

SPtr<Scene> loadSparsePlatformScene(const Context &context) {
   const float SIZE = 30.0f;
   const float DISTANCE = 8.0f;
   const float PLATFORM_HEIGHT = 15.0f;
   const float SPAWN_HEIGHT = PLATFORM_HEIGHT + 5.0f;
   const float platformWidth = 5.0f;
   const glm::vec3 platformScale(platformWidth, 2.0f, platformWidth);

   glm::vec3 spawnLocations[] = {
      glm::vec3(-SIZE, SPAWN_HEIGHT, -SIZE),
      glm::vec3(SIZE - DISTANCE / 2.0f, SPAWN_HEIGHT, -SIZE),
      glm::vec3(-SIZE, SPAWN_HEIGHT, SIZE - DISTANCE / 2.0f),
      glm::vec3(SIZE - DISTANCE / 2.0f, SPAWN_HEIGHT, SIZE - DISTANCE / 2.0f)
   };

   return loadBasicScene(context, spawnLocations, [SIZE, DISTANCE, PLATFORM_HEIGHT, platformScale](Scene &scene, SPtr<Model> boxModel) {
      const float HEIGHT_RANGE = 2.0f;
      std::default_random_engine generator;
      std::uniform_real_distribution<float> distribution(-HEIGHT_RANGE, HEIGHT_RANGE);

      for (float x = -SIZE; x < SIZE; x += DISTANCE) {
         for (float z = -SIZE; z < SIZE; z += DISTANCE) {
            float heightDiff = distribution(generator);
            scene.addObject(createStaticObject(boxModel, glm::vec3(x, PLATFORM_HEIGHT + heightDiff, z), platformScale, 1.0f, 0.3f));
         }
      }
   });
}

SPtr<Scene> loadSpikyScene(const Context &context) {
   glm::vec3 spawnLocations[] = {
      glm::vec3(-10.0f, 55.0f, 0.0f),
      glm::vec3(0.0f, 55.0f, 10.0f),
      glm::vec3(10.0f, 55.0f, 0.0f),
      glm::vec3(0.0f, 55.0f, -10.0f)
   };

   return loadBasicScene(context, spawnLocations, [](Scene &scene, SPtr<Model> boxModel) {
      const float SPIKE_WIDTH = 5.0f;
      // Spawn platforms
      scene.addObject(createStaticObject(boxModel, glm::vec3(-10.0f, 25.0f, 0.0f), glm::vec3(SPIKE_WIDTH, 50.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 25.0f, 10.0f), glm::vec3(SPIKE_WIDTH, 50.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(10.0f, 25.0f, 0.0f), glm::vec3(SPIKE_WIDTH, 50.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 25.0f, -10.0f), glm::vec3(SPIKE_WIDTH, 50.0f, SPIKE_WIDTH), 1.0f, 0.3f));

      // Lower platforms
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));

      scene.addObject(createStaticObject(boxModel, glm::vec3(-10.0f, 20.0f, -10.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(-10.0f, 20.0f, 10.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(10.0f, 20.0f, -10.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(10.0f, 20.0f, 10.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));

      scene.addObject(createStaticObject(boxModel, glm::vec3(-20.0f, 20.0f, 0.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 20.0f, 20.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(20.0f, 20.0f, 0.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 20.0f, -20.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));

      scene.addObject(createStaticObject(boxModel, glm::vec3(-20.0f, 20.0f, -20.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(-20.0f, 20.0f, 20.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(20.0f, 20.0f, -20.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(SPIKE_WIDTH, 40.0f, SPIKE_WIDTH), 1.0f, 0.3f));
   });
}

SPtr<Scene> loadFourTowersScene(const Context &context) {
   glm::vec3 spawnLocations[] = {
      glm::vec3(-30.0f, 23.0f, 0.0f),
      glm::vec3(0.0f, 23.0f, 30.0f),
      glm::vec3(30.0f, 23.0f, 0.0f),
      glm::vec3(0.0f, 23.0f, -30.0f)
   };

   return loadBasicScene(context, spawnLocations, [](Scene &scene, SPtr<Model> boxModel) {
      // Spawn platforms
      scene.addObject(createStaticObject(boxModel, glm::vec3(-30.0f, 5.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(-30.0f, 15.0f, 0.0f), glm::vec3(3.0f, 9.0f, 3.0f), 1.0f, 0.3f));
      buildTower(scene, boxModel, glm::vec3(-30.0f, 9.6f, 0.0f), 0.4f, 2.0f, 2.0f, 23);

      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 5.0f, 30.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 15.0f, 30.0f), glm::vec3(3.0f, 9.0f, 3.0f), 1.0f, 0.3f));
      buildTower(scene, boxModel, glm::vec3(0.0f, 9.6f, 30.0f), 0.4f, 2.0f, 2.0f, 23);

      scene.addObject(createStaticObject(boxModel, glm::vec3(30.0f, 5.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(30.0f, 15.0f, 0.0f), glm::vec3(3.0f, 9.0f, 3.0f), 1.0f, 0.3f));
      buildTower(scene, boxModel, glm::vec3(30.0f, 9.6f, 0.0f), 0.4f, 2.0f, 2.0f, 23);

      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 5.0f, -30.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 15.0f, -30.0f), glm::vec3(3.0f, 9.0f, 3.0f), 1.0f, 0.3f));
      buildTower(scene, boxModel, glm::vec3(0.0f, 9.6f, -30.0f), 0.4f, 2.0f, 2.0f, 23);

      // Platforms between
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(50.0f, 10.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(3.0f, 10.0f, 50.0f), 1.0f, 0.3f));

      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 5.0f, -30.0f), glm::vec3(50.0f, 10.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 5.0f, 30.0f), glm::vec3(50.0f, 10.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(-30.0f, 5.0f, 0.0f), glm::vec3(3.0f, 10.0f, 50.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(30.0f, 5.0f, 0.0f), glm::vec3(3.0f, 10.0f, 50.0f), 1.0f, 0.3f));

      // Four corners
      scene.addObject(createStaticObject(boxModel, glm::vec3(-30.0f, 5.0f, -30.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(-30.0f, 5.0f, 30.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(30.0f, 5.0f, -30.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(30.0f, 5.0f, 30.0f), glm::vec3(10.0f, 10.0f, 10.0f), 1.0f, 0.3f));
   });
}


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
      glm::vec3(-10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, 10.0f),
      glm::vec3(10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, -10.0f)
   };

   return loadBasicScene(context, spawnLocations, [&context](Scene &scene, SPtr<Model> boxModel) {
      AssetManager &assetManager = context.getAssetManager();
      SPtr<ShaderProgram> phongShaderProgram(assetManager.loadShaderProgram("shaders/phong"));

      SPtr<PhongMaterial> floorMaterial = createPhongMaterial(glm::vec3(0.78f, 0.60f, 0.34f) * 1.5f, 0.2f, 5.0f);
      SPtr<Mesh> floorMesh = assetManager.loadMesh("meshes/test.obj");
      SPtr<Model> floorModel(std::make_shared<Model>(phongShaderProgram, floorMesh));
      floorModel->attachMaterial(floorMaterial);
      scene.addObject(createBvhObject(floorModel, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f), 1.0f, 0.3f));

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

      glm::vec3 loc(22.0f, 17.6f, 31.0f);
      scene.addObject(createStaticObject(trunkModel, loc, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc2(-37.8f, 15.0f, -12.8f);
      scene.addObject(createStaticObject(trunkModel, loc2, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc2, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc3(-31.9f, 6.1f, 15.4f);
      scene.addObject(createStaticObject(trunkModel, loc3, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc3, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc4(1.0f, 1.8f, -6.8f);
      scene.addObject(createStaticObject(trunkModel, loc4, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc4, glm::vec3(1.0f), 1.0f, 0.3f));

      glm::vec3 loc5(40.3f, 1.9f, -32.1f);
      scene.addObject(createStaticObject(trunkModel, loc5, glm::vec3(1.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(leavesModel, loc5, glm::vec3(1.0f), 1.0f, 0.3f));

      // Rocks

      glm::vec3 loc6(13.0f, 7.6f, 10.0f);
      scene.addObject(createStaticObject(rockModel, loc6, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc7(-10.0f, 3.0f, 6.0f);
      scene.addObject(createStaticObject(rockModel, loc7, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc8(30.0f, 2.5f, -9.0f);
      scene.addObject(createStaticObject(rockModel, loc8, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc9(-33.7f, 15.0f, -13.3f);
      scene.addObject(createStaticObject(rockModel, loc9, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));

      glm::vec3 loc10(-8.0f, 4.6f, 33.0f);
      scene.addObject(createStaticObject(rockModel, loc10, glm::vec3(randomScale(0.5f, 2.0f)), 1.0f, 0.3f, randomOrientation()));
   });
}

SPtr<Scene> loadNextScene(const Context &context) {
   static std::vector<std::function<SPtr<Scene> (const Context &context)>> loadFunctions;
   static int index = 0;

   if (loadFunctions.empty()) {
      loadFunctions.push_back(loadTestScene);
      loadFunctions.push_back(loadCenterPlatformScene);
      loadFunctions.push_back(loadMiniTowersScene);
      loadFunctions.push_back(loadSparsePlatformScene);
      loadFunctions.push_back(loadSpikyScene);
      loadFunctions.push_back(loadFourTowersScene);
   }

   std::function<SPtr<Scene> (const Context &context)> function = loadFunctions[index];
   index = (index + 1) % loadFunctions.size();

   return function(context);
}

} // namespace SceneLoader
