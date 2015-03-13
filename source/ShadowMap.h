#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "GLIncludes.h"

#include <glm/glm.hpp>

class ShaderProgram;

class ShadowMap {
protected:
   GLuint framebufferID;
   GLuint textureID;
   bool cube;
   int size;
   SPtr<ShaderProgram> shadowProgram;

public:
   ShadowMap();

   virtual ~ShadowMap();

   void init(int size);

   void initCube(int size);

   void enable();

   void disable();

   bool isCube() const {
      return cube;
   }

   GLuint getTextureID() const {
      return textureID;
   }

   glm::mat4 getProjectionMatrix() const;

   glm::mat4 getBiasedProjectionMatrix() const;

   glm::mat4 getViewMatrix() const;

   SPtr<ShaderProgram> getShadowProgram() const;
};

#endif
