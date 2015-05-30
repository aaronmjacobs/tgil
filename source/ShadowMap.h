#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include "GLIncludes.h"

#include <glm/glm.hpp>

class ShaderProgram;
class Texture;

class ShadowMap {
protected:
   GLuint framebufferID;
   SPtr<Texture> texture;
   bool cube;
   int size;
   SPtr<ShaderProgram> shadowProgram;

public:
   static const int MAX_SHADOWS = 4;
   static const int MAX_LARGE_SHADOWS = 1;
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

   SPtr<ShaderProgram> getShadowProgram() const;
};

class ShadowMapManager {
protected:
   std::vector<SPtr<ShadowMap>> standardShadowMaps;
   std::vector<SPtr<ShadowMap>> largeShadowMaps;
   std::vector<SPtr<ShadowMap>> cubeShadowMaps;

   SPtr<ShadowMap> getShadowMap(const std::vector<SPtr<ShadowMap>> &maps);

public:
   ShadowMapManager(int numStandard, int numLarge, int numCube, int standardSize = 1024, int largeSize = 4096, int cubeSize = 512);

   virtual ~ShadowMapManager();

   SPtr<ShadowMap> getFreeStandardMap();

   SPtr<ShadowMap> getFreeLargeMap();

   SPtr<ShadowMap> getFreeCubeMap();
};

#endif
