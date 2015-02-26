#include "DefaultImageSource.h"
#include "FancyAssert.h"
#include "IOUtils.h"
#include "LogHelper.h"
#include "TextureAssetManager.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT ASSERT
#include <stb/stb_image.h>

namespace {

struct ImageInfo {
   int width;
   int height;
   int composition;
   unsigned char *pixels;
};

ImageInfo getDefaultImageInfo() {
   ImageInfo defaultImageInfo;

   defaultImageInfo.pixels = stbi_load_from_memory(DEFAULT_IMAGE_SOURCE, DEFAULT_IMAGE_SOURCE_SIZE, &defaultImageInfo.width, &defaultImageInfo.height, &defaultImageInfo.composition, 0);

   if (!defaultImageInfo.pixels) {
      LOG_FATAL("Unable to load default texture");
   }

   return defaultImageInfo;
}

ImageInfo loadImage(const std::string &fileName) {
   ImageInfo info;

   std::string dataFileName(IOUtils::dataPath(fileName));
   info.pixels = stbi_load(dataFileName.c_str(), &info.width, &info.height, &info.composition, 0);

   if (!info.pixels) {
      LOG_WARNING("Unable to load image from file: " << dataFileName);
      info = getDefaultImageInfo();
   }

   return info;
}

bool getFormat(const ImageInfo &info, GLint *format) {
   switch (info.composition) {
      case 1:
         *format = GL_R8;
         return true;
      case 2:
         *format = GL_RG16;
         return true;
      case 3:
         *format = GL_RGB;
         return true;
      case 4:
         *format = GL_RGBA;
         return true;
      default:
         *format = GL_RGBA;
         return false;
   }
}

} // namespace

TextureAssetManager::TextureAssetManager() {
}

TextureAssetManager::~TextureAssetManager() {
}

GLuint TextureAssetManager::loadTexture(const std::string &fileName, TextureWrap::Type wrap) {
   if (textureMap.count(fileName)) {
      return textureMap.at(fileName);
   }

   ImageInfo info = loadImage(fileName);

   GLint format;
   if (!getFormat(info, &format)) {
      LOG_WARNING("Unsupported image composition for image: " << fileName);

      info = getDefaultImageInfo();
      if (!getFormat(info, &format)) {
         LOG_FATAL("Unsupported image composition for default image");
      }
   }

   GLuint textureID;
   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_2D, textureID);

   glTexImage2D(GL_TEXTURE_2D, 0, format, info.width, info.height, 0, format, GL_UNSIGNED_BYTE, info.pixels);

   // TODO Make configurable
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

   glBindTexture(GL_TEXTURE_2D, 0);

   stbi_image_free(info.pixels);

   textureMap[fileName] = textureID;
   return textureID;
}

GLuint TextureAssetManager::loadCubemap(const std::string &path, const std::string &extension) {
   if (cubemapMap.count(path)) {
      return cubemapMap.at(path);
   }

   const std::string &rightName = "/right." + extension;
   const std::string &leftName = "/left." + extension;
   const std::string &upName = "/up." + extension;
   const std::string &downName = "/down." + extension;
   const std::string &backName = "/back." + extension;
   const std::string &frontName = "/front." + extension;

   ImageInfo right = loadImage(path + rightName);
   ImageInfo left = loadImage(path + leftName);
   ImageInfo up = loadImage(path + upName);
   ImageInfo down = loadImage(path + downName);
   ImageInfo back = loadImage(path + backName);
   ImageInfo front = loadImage(path + frontName);

   GLuint cubemapID;
   glGenTextures(1, &cubemapID);
   glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, right.width, right.height, 0, GL_RGB, GL_UNSIGNED_BYTE, right.pixels);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, left.width, left.height, 0, GL_RGB, GL_UNSIGNED_BYTE, left.pixels);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, up.width, up.height, 0, GL_RGB, GL_UNSIGNED_BYTE, up.pixels);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, down.width, down.height, 0, GL_RGB, GL_UNSIGNED_BYTE, down.pixels);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, back.width, back.height, 0, GL_RGB, GL_UNSIGNED_BYTE, back.pixels);
   glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, front.width, front.height, 0, GL_RGB, GL_UNSIGNED_BYTE, front.pixels);

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

   stbi_image_free(right.pixels);
   stbi_image_free(left.pixels);
   stbi_image_free(up.pixels);
   stbi_image_free(down.pixels);
   stbi_image_free(back.pixels);
   stbi_image_free(front.pixels);

   cubemapMap[path] = cubemapID;
   return cubemapID;
}
