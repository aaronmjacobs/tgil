#include "AssetManager.h"
#include "AudioComponent.h"
#include "AudioManager.h"
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
#include "TextureMaterial.h"
#include "TimeMaterial.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <random>
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

SPtr<ShaderProgram> loadPhongTextureShaderProgram(AssetManager &assetManager) {
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/phong_textured");

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

   shaderProgram->addUniform("uTexture");
   shaderProgram->addAttribute("aTexCoord");

   return shaderProgram;
}

SPtr<ShaderProgram> loadTilingTextureShaderProgram(AssetManager &assetManager) {
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/tiling_texture");

   shaderProgram->addUniform("uProjMatrix");
   shaderProgram->addUniform("uViewMatrix");
   shaderProgram->addUniform("uModelMatrix");

   shaderProgram->addAttribute("aPosition");
   shaderProgram->addAttribute("aNormal");

   shaderProgram->addUniform("uTexture");

   return shaderProgram;
}

SPtr<ShaderProgram> loadLavaShaderProgram(AssetManager &assetManager) {
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/lava");

   shaderProgram->addUniform("uProjMatrix");
   shaderProgram->addUniform("uViewMatrix");
   shaderProgram->addUniform("uModelMatrix");

   shaderProgram->addAttribute("aPosition");
   shaderProgram->addAttribute("aTexCoord");

   shaderProgram->addUniform("uTexture");
   shaderProgram->addUniform("uNoiseTexture");
   shaderProgram->addUniform("uTime");

   return shaderProgram;
}

SPtr<ShaderProgram> loadSkyboxShaderProgram(AssetManager &assetManager) {
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/skybox");

   shaderProgram->addUniform("uProjMatrix");
   shaderProgram->addUniform("uViewMatrix");
   shaderProgram->addUniform("uSkybox");

   shaderProgram->addAttribute("aPosition");

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

   // Audio
   SPtr<AudioComponent> audioComponent(std::make_shared<AudioComponent>(*player));
   audioComponent->registerSoundEvent(Event::STEP, SoundGroup::STEP);
   audioComponent->registerSoundEvent(Event::JUMP, SoundGroup::JUMP);
   player->setAudioComponent(audioComponent);

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

         PlayerLogicComponent *playerLogic = static_cast<PlayerLogicComponent*>(&collidingGameObject->getLogicComponent());
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

   SPtr<ShaderProgram> phongShaderProgram = loadPhongShaderProgram(assetManager);
   SPtr<ShaderProgram> phongTextureShaderProgram = loadPhongTextureShaderProgram(assetManager);
   SPtr<ShaderProgram> tilingTextureShaderProgram = loadTilingTextureShaderProgram(assetManager);
   SPtr<ShaderProgram> lavaShaderProgram = loadLavaShaderProgram(assetManager);
   SPtr<ShaderProgram> skyboxShaderProgram = loadSkyboxShaderProgram(assetManager);

   //GLuint lavaTextureID = assetManager.loadTexture("textures/lava.png", TextureWrap::Repeat);
   GLuint rockTextureID = assetManager.loadTexture("textures/soil.jpg", TextureWrap::Repeat);
   GLuint skyboxID = assetManager.loadCubemap("textures/desert", "jpg");

   GLuint lavatileTextureID = assetManager.loadTexture("textures/lavatile2.jpg", TextureWrap::Repeat);
   GLuint noiseID = assetManager.loadTexture("textures/cloud.png", TextureWrap::Repeat);

   SPtr<PhongMaterial> boxMaterial = createPhongMaterial(*phongShaderProgram, glm::vec3(0.1f, 0.3f, 0.8f), 0.2f, 50.0f);
   //SPtr<PhongMaterial> planeMaterial = createPhongMaterial(*phongShaderProgram, glm::vec3(0.4f), 0.2f, 50.0f);
   SPtr<TextureMaterial> rockMaterial(std::make_shared<TextureMaterial>(*phongTextureShaderProgram, rockTextureID, "uTexture"));
   //SPtr<TextureMaterial> lavaMaterial(std::make_shared<TextureMaterial>(*tilingTextureShaderProgram, lavaTextureID, "uTexture"));
   SPtr<TextureMaterial> skyboxMaterial(std::make_shared<TextureMaterial>(*skyboxShaderProgram, skyboxID, "uSkybox", GL_TEXTURE_CUBE_MAP));
   SPtr<TextureMaterial> lavatileMaterial(std::make_shared<TextureMaterial>(*lavaShaderProgram, lavatileTextureID, "uTexture"));
   SPtr<TextureMaterial> noiseMaterial(std::make_shared<TextureMaterial>(*lavaShaderProgram, noiseID, "uNoiseTexture"));
   SPtr<TimeMaterial> timeMaterial(std::make_shared<TimeMaterial>(*lavaShaderProgram));

   SPtr<Mesh> skyboxMesh = assetManager.loadMesh("meshes/skybox.obj");
   SPtr<Mesh> boxMesh = assetManager.loadMesh("meshes/cube.obj");
   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xz_plane.obj");
   SPtr<Mesh> playerMesh = assetManager.loadMesh("meshes/player.obj");

   SPtr<Model> skyboxModel(std::make_shared<Model>(skyboxShaderProgram, skyboxMaterial, skyboxMesh));
   SPtr<Model> boxModel(std::make_shared<Model>(phongTextureShaderProgram, rockMaterial, boxMesh));
   boxModel->attachMaterial(boxMaterial);
   //SPtr<Model> planeModel(std::make_shared<Model>(phongShaderProgram, planeMaterial, planeMesh));
   SPtr<Model> lavaModel(std::make_shared<Model>(lavaShaderProgram, lavatileMaterial, planeMesh));
   lavaModel->attachMaterial(noiseMaterial);
   lavaModel->attachMaterial(timeMaterial);
   //SPtr<Model> lavalModel(std::make_shared<Model>(tilingTextureShaderProgram, lavaMaterial, planeMesh));

   SPtr<GameObject> skybox(std::make_shared<GameObject>());
   skybox->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*skybox));
   skybox->getGraphicsComponent().setModel(skyboxModel);
   scene->setSkybox(skybox);

   // Light
   float sunDistance = 300.0f;
   glm::vec3 sunPos = glm::vec3(-1.0f, 3.0f, 0.5f) * sunDistance;
   float sunIntensity = 2.0f;
   glm::vec3 sunColor = glm::normalize(glm::vec3(1.0f, 0.95f, 0.75f)) * sunIntensity;
   scene->addLight(createLight(sunPos, sunColor, 0.0f, 0.0f, 0.0f));

   glm::vec3 lavaLightColor = glm::vec3(0.9f, 0.4f, 0.1f) * 0.5f;
   scene->addLight(createLight(glm::vec3(-80.0f, -50.0f, -80.0f), lavaLightColor, 0.0f, 0.0f, 0.0f));
   scene->addLight(createLight(glm::vec3(80.0f, -50.0f, -80.0f), lavaLightColor, 0.0f, 0.0f, 0.0f));
   scene->addLight(createLight(glm::vec3(-80.0f, -50.0f, 80.0f), lavaLightColor, 0.0f, 0.0f, 0.0f));
   scene->addLight(createLight(glm::vec3(80.0f, -50.0f, 80.0f), lavaLightColor, 0.0f, 0.0f, 0.0f));

   // Lava
   float lavaSize = 100.0f;
   scene->addObject(createStaticObject(lavaModel, glm::vec3(0.0f), glm::vec3(lavaSize), 1.0f, 0.3f));
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

SPtr<Scene> loadCenterPlatformScene(const Context &context) {
   glm::vec3 spawnLocations[] = {
      glm::vec3(-10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, 10.0f),
      glm::vec3(10.0f, 20.0f, 0.0f),
      glm::vec3(0.0f, 20.0f, -10.0f)
   };

   return loadBasicScene(context, spawnLocations, [](Scene &scene, SPtr<Model> boxModel) {
      // Single platform
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f), glm::vec3(30.0f, 20.0f, 30.0f), 1.0f, 0.3f));
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
      scene.addObject(createStaticObject(boxModel, glm::vec3(-distanceFromCenter / 2.0f, 0.0f, 0.0f), glm::vec3(distanceFromCenter, 18.0f, 0.5f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 0.0f, distanceFromCenter / 2.0f), glm::vec3(0.5f, 18.0f, distanceFromCenter), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(distanceFromCenter / 2.0f, 0.0f, 0.0f), glm::vec3(distanceFromCenter, 18.0f, 0.5f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 0.0f, -distanceFromCenter / 2.0f), glm::vec3(0.5f, 18.0f, distanceFromCenter), 1.0f, 0.3f));
   });
}

SPtr<Scene> loadSparsePlatformScene(const Context &context) {
   const float SIZE = 30.0f;
   const float DISTANCE = 8.0f;
   const float PLATFORM_HEIGHT = 15.0f;
   const float SPAWN_HEIGHT = PLATFORM_HEIGHT + 5.0f;
   const float platformWidth = 3.0f;
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
      // Spawn platforms
      scene.addObject(createStaticObject(boxModel, glm::vec3(-10.0f, 25.0f, 0.0f), glm::vec3(3.0f, 50.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 25.0f, 10.0f), glm::vec3(3.0f, 50.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(10.0f, 25.0f, 0.0f), glm::vec3(3.0f, 50.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 25.0f, -10.0f), glm::vec3(3.0f, 50.0f, 3.0f), 1.0f, 0.3f));

      // Lower platforms
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));

      scene.addObject(createStaticObject(boxModel, glm::vec3(-10.0f, 20.0f, -10.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(-10.0f, 20.0f, 10.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(10.0f, 20.0f, -10.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(10.0f, 20.0f, 10.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));

      scene.addObject(createStaticObject(boxModel, glm::vec3(-20.0f, 20.0f, 0.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 20.0f, 20.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(20.0f, 20.0f, 0.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(0.0f, 20.0f, -20.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));

      scene.addObject(createStaticObject(boxModel, glm::vec3(-20.0f, 20.0f, -20.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(-20.0f, 20.0f, 20.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(20.0f, 20.0f, -20.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
      scene.addObject(createStaticObject(boxModel, glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(3.0f, 40.0f, 3.0f), 1.0f, 0.3f));
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

SPtr<Scene> loadNextScene(const Context &context) {
   static std::vector<std::function<SPtr<Scene> (const Context &context)>> loadFunctions;
   static int index = 0;

   if (loadFunctions.empty()) {
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
