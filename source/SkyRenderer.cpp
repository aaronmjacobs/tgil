#include "AssetManager.h"
#include "Context.h"
#include "GameObject.h"
#include "LightComponent.h"
#include "Model.h"
#include "Renderer.h"
#include "RenderData.h"
#include "ShaderProgram.h"
#include "SkyRenderer.h"
#include "TextureMaterial.h"

SkyRenderer::SkyRenderer() {
}

SkyRenderer::~SkyRenderer() {
}

void SkyRenderer::loadPlane() {
   AssetManager &assetManager = Context::getInstance().getAssetManager();
   SPtr<ShaderProgram> shaderProgram = assetManager.loadShaderProgram("shaders/sky");

   SPtr<Mesh> planeMesh = assetManager.getMeshForShape(MeshShape::XYPlane);
   xyPlane = UPtr<Model>(new Model(shaderProgram, planeMesh));

   SPtr<Texture> spaceTexture = assetManager.loadCubemap("textures/space");
   SPtr<TextureMaterial> textureMaterial(std::make_shared<TextureMaterial>(spaceTexture, "uTexture"));
   xyPlane->attachMaterial(textureMaterial);
}

void SkyRenderer::init() {
   loadPlane();
}

void SkyRenderer::render(const glm::mat4 &view, const glm::mat4 &proj, const Viewport &viewport, const glm::vec2 &framebufferResolution, SPtr<GameObject> sun) {
   const glm::vec3 &pos = -sun->getLightComponent().getDirection();
   xyPlane->getShaderProgram()->setUniformValue("uLightDir", pos);

   xyPlane->getShaderProgram()->setUniformValue("uInvProjMatrix", glm::inverse(proj));
   xyPlane->getShaderProgram()->setUniformValue("uInvViewMatrix", glm::inverse(view));
   xyPlane->getShaderProgram()->setUniformValue("uViewport", glm::vec4(viewport.x, viewport.y, viewport.width, viewport.height));
   xyPlane->getShaderProgram()->setUniformValue("uFramebufferResolution", framebufferResolution);

   RenderData renderData;
   xyPlane->draw(renderData);
}