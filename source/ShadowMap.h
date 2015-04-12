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
   static const int MAX_SHADOWS = 5;
   static const int MAX_CUBE_SHADOWS = 4;

   ShadowMap();

   virtual ~ShadowMap();

   void init(int size);

   void initCube(int size);

   void setActiveFace(int face);

   void enable();

   void disable();

   GLenum bindTexture();

   bool isCube() const {
      return cube;
   }

   GLuint getTextureID() const {
      return textureID;
   }

   SPtr<ShaderProgram> getShadowProgram() const;
};

class ShadowMapManager {
protected:
   std::vector<SPtr<ShadowMap>> standardShadowMaps;
   std::vector<SPtr<ShadowMap>> cubeShadowMaps;

   SPtr<ShadowMap> getShadowMap(const std::vector<SPtr<ShadowMap>> &maps);

public:
   ShadowMapManager(int numStandard, int numCube, int standardSize = 4096, int cubeSize = 512);

   virtual ~ShadowMapManager();

   SPtr<ShadowMap> getFreeStandardMap();

   SPtr<ShadowMap> getFreeCubeMap();
};

#endif
