#include "AssetManager.h"
#include "Context.h"
#include "FancyAssert.h"
#include "HUDRenderer.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "TextureMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

HUDRenderer::HUDRenderer() {
}

HUDRenderer::~HUDRenderer() {
}

void HUDRenderer::loadPlane() {
   AssetManager &assetManager = Context::getInstance().getAssetManager();
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/hud");
   shaderProgram->addUniform("uTexture");
   shaderProgram->addUniform("uTransform");
   shaderProgram->addAttribute("aPosition");
   shaderProgram->addAttribute("aTexCoord");
   uTransform = shaderProgram->getUniform("uTransform");
   textureMaterial = std::make_shared<TextureMaterial>(*shaderProgram, 0, "uTexture");

   // TODO Hardcode the mesh
   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xy_plane.obj");
   xyPlane = UPtr<Model>(new Model(shaderProgram, textureMaterial, planeMesh));

   GLuint crosshairID = assetManager.loadTexture("textures/crosshair.png");
   HUDElement element = { crosshairID, 50.0f, 50.0f, 3.0f };
   attach(element);
}

void HUDRenderer::init() {
   loadPlane();
}

void HUDRenderer::attach(const HUDElement &element) {
   elements.push_back(element);
}

void HUDRenderer::render(int width, int height) {
   ASSERT(width > 0 && height > 0, "Width and height must be positive");

   for (const HUDElement &element : elements) {
      xyPlane->getShaderProgram()->use();

      textureMaterial->setTextureID(element.textureID);

      glm::mat4 scale(glm::scale(glm::vec3(element.scale) / 100.0f));
      glm::mat4 ratio(glm::scale(glm::vec3((float)height / width, 1.0f, 1.0f)));
      glm::mat4 translate(glm::translate(glm::vec3((element.x / 50.0f) - 1.0f, (element.y / 50.0f) - 1.0f, 0.0f)));

      glm::mat4 transform = translate * ratio * scale;
      glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transform));

      xyPlane->draw();
   }
}
