#include "AssetManager.h"
#include "Context.h"
#include "Material.h"
#include "Model.h"
#include "PostProcessRenderer.h"
#include "RenderData.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

class TintMaterial : public Material {
protected:
   float opacity;
   glm::vec3 tint;

public:
   TintMaterial();

   virtual ~TintMaterial();

   virtual void apply(ShaderProgram &shaderProgram);

   virtual void disable();

   void setValues(float opacity, const glm::vec3 &tint);
};

TintMaterial::TintMaterial() {
   opacity = 1.0f;
   tint = glm::vec3(0.0f);
}

TintMaterial::~TintMaterial() {
}

void TintMaterial::apply(ShaderProgram &shaderProgram) {
   shaderProgram.setUniformValue("uOpacity", opacity);
   shaderProgram.setUniformValue("uTint", tint);
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

   material = std::make_shared<TintMaterial>();

   SPtr<Mesh> planeMesh = assetManager.getMeshForShape(MeshShape::XYPlane);
   xyPlane = UPtr<Model>(new Model(shaderProgram, planeMesh));
   xyPlane->attachMaterial(material);
}

void PostProcessRenderer::init() {
   loadPlane();
}

void PostProcessRenderer::render(float opacity, const glm::vec3 &tint) {
   material->setValues(opacity, tint);

   material->apply(*xyPlane->getShaderProgram());

   RenderData renderData;
   xyPlane->draw(renderData);
}
