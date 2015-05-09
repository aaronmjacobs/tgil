#ifndef TEXTURE_MATERIAL_H
#define TEXTURE_MATERIAL_H

#include "GLIncludes.h"
#include "Material.h"

class ShaderProgram;
class Texture;

class TextureMaterial : public Material {
protected:
   // The texture
   SPtr<Texture> texture;

   // The texture unit
   GLenum textureUnit;

   // Name of the texture uniform
   std::string textureUniformName;

public:
   TextureMaterial(SPtr<Texture> texture, const std::string &textureUniformName);

   virtual ~TextureMaterial();

   virtual void apply(ShaderProgram &shaderProgram);

   virtual void disable();

   void setTexture(SPtr<Texture> texture);
};

#endif
