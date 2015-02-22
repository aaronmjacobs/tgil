#ifndef TEXTURE_ASSET_MANAGER_H
#define TEXTURE_ASSET_MANAGER_H

#include "GLIncludes.h"

#include <string>
#include <unordered_map>

namespace TextureWrap {

enum Type {
   Edge = GL_CLAMP_TO_EDGE,
   Border = GL_CLAMP_TO_BORDER,
   Repeat = GL_REPEAT,
   MirroredRepeat = GL_MIRRORED_REPEAT
};

} // namespace TextureWrap

typedef std::unordered_map<std::string, GLuint> TextureMap;
typedef std::unordered_map<std::string, GLuint> CubemapMap;

class TextureAssetManager {
protected:
   TextureMap textureMap;
   CubemapMap cubemapMap;

public:
   TextureAssetManager();

   virtual ~TextureAssetManager();

   GLuint loadTexture(const std::string &fileName, TextureWrap::Type wrap);

   GLuint loadCubemap(const std::string &path, const std::string &extension);
};

#endif
