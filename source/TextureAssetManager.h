#ifndef TEXTURE_ASSET_MANAGER_H
#define TEXTURE_ASSET_MANAGER_H

#include "GLIncludes.h"
#include "Texture.h"

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

typedef std::unordered_map<std::string, SPtr<Texture>> TextureMap;
typedef std::unordered_map<std::string, SPtr<Texture>> CubemapMap;

class TextureAssetManager {
protected:
   TextureMap textureMap;
   CubemapMap cubemapMap;

public:
   TextureAssetManager();

   virtual ~TextureAssetManager();

   SPtr<Texture> loadTexture(const std::string &fileName, TextureWrap::Type wrap);

   SPtr<Texture> loadCubemap(const std::string &path, const std::string &extension);
};

#endif
