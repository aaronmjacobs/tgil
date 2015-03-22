#include "AssetManager.h"
#include "Context.h"
#include "FancyAssert.h"
#include "ShaderProgram.h"
#include "ShadowMap.h"

#include <glm/gtc/matrix_transform.hpp>

ShadowMap::ShadowMap()
   : cube(false) {
   glGenFramebuffers(1, &framebufferID);
   glGenTextures(1, &textureID);
}

ShadowMap::~ShadowMap() {
   glDeleteTextures(1, &textureID);
   glDeleteFramebuffers(1, &framebufferID);
}

void ShadowMap::init(int size) {
   shadowProgram = Context::getInstance().getAssetManager().loadShaderProgram("shaders/shadow");

   cube = false;
   this->size = size;

   glBindTexture(GL_TEXTURE_2D, textureID);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

   glBindTexture(GL_TEXTURE_2D, 0);

   glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);

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

   glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

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

   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

   glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

   // Disable writes and reads to / from the color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::enable() {
   glViewport(0, 0, size, size);
   glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}

void ShadowMap::disable() {
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 ShadowMap::getProjectionMatrix() const {
   const float width = 30.0f;
   const float depth = 100.0f;

   return glm::ortho<float>(-width, width, -width, width, -width, depth);
}

glm::mat4 ShadowMap::getBiasedProjectionMatrix() const {
   const glm::mat4 bias = {
      0.5f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.5f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.5f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f };

   return bias * getProjectionMatrix();
}

glm::mat4 ShadowMap::getViewMatrix() const {
   const float sunDistance = 50.0f;
   const glm::vec3 sunDir(1.0f, -1.0f, -0.5f);
   const glm::vec3 pos = glm::normalize(-sunDir) * sunDistance;

   return glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

SPtr<ShaderProgram> ShadowMap::getShadowProgram() const {
   return shadowProgram;
}
