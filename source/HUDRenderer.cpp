#include "Ability.h"
#include "AssetManager.h"
#include "Context.h"
#include "FancyAssert.h"
#include "HUDRenderer.h"
#include "Model.h"
#include "PlayerLogicComponent.h"
#include "ShaderProgram.h"
#include "TextureMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace {

const float DEFAULT_OPACITY = 0.5f;
const glm::vec3 DEFAULT_TINT(1.0f);
const glm::vec3 DEFAULT_FILL(1.0f);

const float CROSSHAIR_SCALE = 3.0f;
const float COOLDOWN_SCALE = 7.0f;
const float COOLDOWN_OPACITY = 0.5f;
const glm::vec3 COOLDOWN_OCCURING_COLOR(1.0f);
const glm::vec3 COOLDOWN_OVER_COLOR(0.2f, 1.0f, 0.2f);

std::function<void(HUDElement &element, const PlayerLogicComponent &playerLogic)> getFillUpdateLogic(bool primary) {
   return [primary](HUDElement &element, const PlayerLogicComponent &playerLogic) {
      const Ability &ability = primary ? playerLogic.getPrimaryAbility() : playerLogic.getSecondaryAbility();

      float cooldownTime = ability.getCooldownTime();
      float remainingCooldown = ability.getRemainingCooldownTime();
      float cooldownPercent = cooldownTime == 0.0f ? 1.0f : 1.0f - remainingCooldown / cooldownTime;

      element.fill = glm::vec2(cooldownPercent, 1.0f);
      element.opacity = COOLDOWN_OPACITY * cooldownPercent;
      element.tint = ability.isOnCooldown() ? COOLDOWN_OCCURING_COLOR : COOLDOWN_OVER_COLOR;
   };
}

} // namespace

HUDElement::HUDElement(GLuint textureID, glm::vec2 position, glm::vec2 scale)
: textureID(textureID), position(position), scale(scale), fill(DEFAULT_FILL), tint(DEFAULT_TINT), opacity(DEFAULT_OPACITY), updateLogic(nullptr) {
}

HUDElement::~HUDElement() {
}

void HUDElement::update(const PlayerLogicComponent &playerLogic) {
   if (updateLogic) {
      updateLogic(*this, playerLogic);
   }
}

HUDRenderer::HUDRenderer() {
}

HUDRenderer::~HUDRenderer() {
}

void HUDRenderer::loadPlane() {
   AssetManager &assetManager = Context::getInstance().getAssetManager();
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/hud");
   shaderProgram->addUniform("uTexture");
   shaderProgram->addUniform("uTransform");
   shaderProgram->addUniform("uOpacity");
   shaderProgram->addUniform("uTint");
   shaderProgram->addUniform("uFill");
   uTransform = shaderProgram->getUniform("uTransform");
   uOpacity = shaderProgram->getUniform("uOpacity");
   uTint = shaderProgram->getUniform("uTint");
   uFill = shaderProgram->getUniform("uFill");
   textureMaterial = std::make_shared<TextureMaterial>(*shaderProgram, 0, "uTexture");

   // TODO Hardcode the mesh
   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xy_plane.obj");
   xyPlane = UPtr<Model>(new Model(shaderProgram, planeMesh));
   xyPlane->attachMaterial(textureMaterial);
}

void HUDRenderer::loadElements() {
   AssetManager &assetManager = Context::getInstance().getAssetManager();

   GLuint crosshairID = assetManager.loadTexture("textures/crosshair.png");
   GLuint throwID = assetManager.loadTexture("textures/hud_throw.png");
   GLuint shoveID = assetManager.loadTexture("textures/hud_shove.png");
   GLuint fillID = assetManager.loadTexture("textures/hud_fill.png");

   HUDElement crosshairElement(crosshairID, glm::vec2(50.0f), glm::vec2(CROSSHAIR_SCALE));
   HUDElement throwElement(throwID, glm::vec2(89.0f, 90.0f), glm::vec2(COOLDOWN_SCALE));
   HUDElement shoveElement(shoveID, glm::vec2(94.0f, 90.0f), glm::vec2(COOLDOWN_SCALE));
   HUDElement throwFillElement(fillID, glm::vec2(89.0f, 90.0f), glm::vec2(COOLDOWN_SCALE));
   HUDElement shoveFillElement(fillID, glm::vec2(94.0f, 90.0f), glm::vec2(COOLDOWN_SCALE));

   throwFillElement.setUpdateLogic(getFillUpdateLogic(true));
   shoveFillElement.setUpdateLogic(getFillUpdateLogic(false));

   attach(crosshairElement);
   attach(throwElement);
   attach(shoveElement);
   attach(throwFillElement);
   attach(shoveFillElement);
}

void HUDRenderer::init() {
   loadPlane();
   loadElements();
}

void HUDRenderer::attach(const HUDElement &element) {
   elements.push_back(element);
}

void HUDRenderer::render(const PlayerLogicComponent &playerLogic, int width, int height) {
   ASSERT(width > 0 && height > 0, "Width and height must be positive");

   for (HUDElement &element : elements) {
      xyPlane->getShaderProgram()->use();

      element.update(playerLogic);
      textureMaterial->setTextureID(element.textureID);

      glUniform1f(uOpacity, element.opacity);
      glUniform2fv(uFill, 1, glm::value_ptr(element.fill));
      glUniform3fv(uTint, 1, glm::value_ptr(element.tint));

      glm::mat4 scale(glm::scale(glm::vec3(element.scale.x, element.scale.y, 1.0f) / 100.0f));
      glm::mat4 ratio(glm::scale(glm::vec3((float)height / width, 1.0f, 1.0f)));
      glm::mat4 translate(glm::translate(glm::vec3((element.position.x / 50.0f) - 1.0f, (element.position.y / 50.0f) - 1.0f, 0.0f)));

      glm::mat4 transform = translate * ratio * scale;
      glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transform));

      xyPlane->draw();
   }
}
