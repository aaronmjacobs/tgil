#include "AssetManager.h"
#include "Context.h"
#include "FancyAssert.h"
#include "ShaderProgram.h"
#include "ShadowMap.h"
#include "TextureUnitManager.h"

#include <glm/gtc/matrix_transform.hpp>

// ShadowMap

ShadowMap::ShadowMap()
   : cube(false) {
   glGenFramebuffers(1, &framebufferID);
}

ShadowMap::~ShadowMap() {
   glDeleteFramebuffers(1, &framebufferID);
}

void ShadowMap::init(int size) {
   shadowProgram = Context::getInstance().getAssetManager().loadShaderProgram("shaders/shadow");

   cube = false;
   this->size = size;

   texture = std::make_shared<Texture>(GL_TEXTURE_2D);
   texture->bind();

   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

   texture->unbind();

   glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->id(), 0);

   // Disable writes and reads to / from the color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);

   ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete");

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::initCube(int size) {
   shadowProgram = Context::getInstance().getAssetManager().loadShaderProgram("shaders/shadow");

   cube = true;
   this->size = size;

   texture = std::make_shared<Texture>(GL_TEXTURE_CUBE_MAP);
   texture->bind();

   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

   texture->unbind();

   glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture->id(), 0);

   // Disable writes and reads to / from the color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);

   ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete");

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::setActiveFace(int face) {
   ASSERT(cube, "Trying to set active face of non-cube shadow map");
   ASSERT(face >= 0 && face < 6, "Invalid face index");

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->id(), 0);
}

void ShadowMap::enable() {
   glViewport(0, 0, size, size);
   glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}

void ShadowMap::disable() {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLenum ShadowMap::bindTexture() {
   GLenum textureUnit = Context::getInstance().getTextureUnitManager().get();

   glActiveTexture(GL_TEXTURE0 + textureUnit);
   texture->bind();

   return textureUnit;
}

SPtr<ShaderProgram> ShadowMap::getShadowProgram() const {
   return shadowProgram;
}

// ShadowMapManager

ShadowMapManager::ShadowMapManager(int numStandard, int numLarge, int numCube, int standardSize, int largeSize, int cubeSize) {
   ASSERT(numStandard >= 0 && numLarge >=0 && numCube >= 0 && standardSize > 0 && largeSize > 0 && cubeSize > 0, "Invalid ShadowMapManager initialization values");

   for (unsigned int i = 0; i < numStandard; ++i) {
      SPtr<ShadowMap> shadowMap(std::make_shared<ShadowMap>());
      shadowMap->init(standardSize);
      standardShadowMaps.push_back(shadowMap);
   }

   for (unsigned int i = 0; i < numLarge; ++i) {
      SPtr<ShadowMap> shadowMap(std::make_shared<ShadowMap>());
      shadowMap->init(largeSize);
      largeShadowMaps.push_back(shadowMap);
   }

   for (unsigned int i = 0; i < numCube; ++i) {
      SPtr<ShadowMap> shadowMap(std::make_shared<ShadowMap>());
      shadowMap->initCube(cubeSize);
      cubeShadowMaps.push_back(shadowMap);
   }
}

ShadowMapManager::~ShadowMapManager() {
}

SPtr<ShadowMap> ShadowMapManager::getShadowMap(const std::vector<SPtr<ShadowMap>> &maps) {
   for (const SPtr<ShadowMap> &shadowMap : maps) {
      if (shadowMap.use_count() == 1) {
         return shadowMap;
      }
   }

   return nullptr;
}

SPtr<ShadowMap> ShadowMapManager::getFreeStandardMap() {
   return getShadowMap(standardShadowMaps);
}

SPtr<ShadowMap> ShadowMapManager::getFreeLargeMap() {
   return getShadowMap(largeShadowMaps);
}

SPtr<ShadowMap> ShadowMapManager::getFreeCubeMap() {
   return getShadowMap(cubeShadowMaps);
}
