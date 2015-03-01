#include "AssetManager.h"
#include "Context.h"
#include "Material.h"
#include "Model.h"
#include "PostProcessRenderer.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

class TintMaterial : public Material {
protected:
   GLint uOpacity;
   GLint uTint;
   float opacity;
   glm::vec3 tint;

public:
   TintMaterial(const ShaderProgram &shaderProgram);

   virtual ~TintMaterial();

   virtual void apply(const Mesh &mesh);

   virtual void disable();

   void setValues(float opacity, const glm::vec3 &tint);
};

TintMaterial::TintMaterial(const ShaderProgram &shaderProgram) {
   uOpacity = shaderProgram.getUniform("uOpacity");
   uTint = shaderProgram.getUniform("uTint");

   opacity = 1.0f;
   tint = glm::vec3(0.0f);
}

TintMaterial::~TintMaterial() {
}

void TintMaterial::apply(const Mesh &mesh) {
   glUniform1f(uOpacity, opacity);
   glUniform3fv(uTint, 1, glm::value_ptr(tint));
}

void TintMaterial::disable() {
}

void TintMaterial::setValues(float opacity, const glm::vec3 &tint) {
   this->opacity = opacity;
   this->tint = tint;
}

PostProcessRenderer::PostProcessRenderer() {
}

PostProcessRenderer::~PostProcessRenderer() {
}

void PostProcessRenderer::loadPlane() {
   AssetManager &assetManager = Context::getInstance().getAssetManager();
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/tint");
   shaderProgram->addUniform("uOpacity");
   shaderProgram->addUniform("uTint");
   shaderProgram->addAttribute("aPosition");

   material = std::make_shared<TintMaterial>(*shaderProgram);

   // TODO Hardcode the mesh
   SPtr<Mesh> planeMesh = assetManager.loadMesh("meshes/xy_plane.obj");
   xyPlane = UPtr<Model>(new Model(shaderProgram, material, planeMesh));
}

void PostProcessRenderer::init() {
   loadPlane();
}

void PostProcessRenderer::render(float opacity, const glm::vec3 &tint) {
   material->setValues(opacity, tint);

   xyPlane->getShaderProgram()->use();

   material->apply(xyPlane->getMesh());

   xyPlane->draw();
}
