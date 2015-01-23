#include "AssetManager.h"
#include "Context.h"
#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "HumanInputComponent.h"
#include "InputHandler.h"
#include "LogHelper.h"
#include "MeshPhysicsComponent.h"
#include "Model.h"
#include "PhongMaterial.h"
#include "PlayerCameraComponent.h"
#include "PlayerLogicComponent.h"
#include "PlayerPhysicsComponent.h"
#include "PointLightComponent.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include <bullet/btBulletDynamicsCommon.h>
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
   staticObject->setPhysicsComponent(std::make_shared<MeshPhysicsComponent>(*staticObject, 0.0f, CollisionGroup::StaticBodies, CollisionGroup::Everything));
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
   dynamicObject->setPhysicsComponent(std::make_shared<MeshPhysicsComponent>(*dynamicObject, mass, CollisionGroup::StaticBodies, CollisionGroup::Everything));
   btRigidBody *gameObjectRigidBody = dynamic_cast<btRigidBody*>(dynamicObject->getPhysicsComponent().getCollisionObject());
   gameObjectRigidBody->setFriction(friction);
   gameObjectRigidBody->setRollingFriction(friction);
   gameObjectRigidBody->setRestitution(restitution);
      
   return dynamicObject;
}

SPtr<GameObject> createPlayer(SPtr<Model> model, const glm::vec3 &position) {
   SPtr<GameObject> player(std::make_shared<GameObject>());

   // Transform
   player->setPosition(position);

   // Graphics
   if (model) {
      player->setGraphicsComponent(std::make_shared<GeometricGraphicsComponent>(*player));
      player->getGraphicsComponent().setModel(model);
   }

   // Physics
   player->setPhysicsComponent(std::make_shared<PlayerPhysicsComponent>(*player, 1.0f));
   btRigidBody *gameObjectRigidBody = dynamic_cast<btRigidBody*>(player->getPhysicsComponent().getCollisionObject());
   gameObjectRigidBody->setFriction(3.0f);
   gameObjectRigidBody->setRollingFriction(3.0f);
   gameObjectRigidBody->setRestitution(0.0f);

   // Camera
   player->setCameraComponent(std::make_shared<PlayerCameraComponent>(*player));

   // Input
   player->setInputComponent(std::make_shared<HumanInputComponent>(*player));

   // Logic
   player->setLogicComponent(std::make_shared<PlayerLogicComponent>(*player));
      
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

   SPtr<Model> boxModel(std::make_shared<Model>(phongShaderProgram, boxMaterial, boxMesh));
   SPtr<Model> planeModel(std::make_shared<Model>(phongShaderProgram, planeMaterial, planeMesh));

   // Light
   scene->addLight(createLight(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.7f), 0.001f, 0.0005f, 0.0001f));

   // Ground plane
   scene->addObject(createStaticObject(planeModel, glm::vec3(0.0f), glm::vec3(100.0f), 1.0f, 0.3f));

   // Boxes
   scene->addObject(createDynamicObject(boxModel, glm::vec3(-5.0f, 5.0f, -10.0f), glm::vec3(1.0f), 0.3f, 0.8f, 1.0f));

   // Players
   for (int i = 0; i < context.getInputHandler().getNumberOfPlayers(); ++i) {
      scene->addCamera(createPlayer(nullptr, glm::vec3(5.0f * i, 1.0f, 0.0f)));
   }

   return scene;
}

} // namespace SceneLoader
