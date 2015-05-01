#include "AssetManager.h"
#include "Context.h"
#include "Model.h"
#include "PostProcessRenderer.h"
#include "RenderData.h"
#include "ShaderProgram.h"
#include "TintMaterial.h"

#include <glm/gtc/type_ptr.hpp>

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
