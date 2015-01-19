#ifndef TEXTURE_MATERIAL_H
#define TEXTURE_MATERIAL_H

#include "GLIncludes.h"
#include "Material.h"

class ShaderProgram;

class TextureMaterial : public Material {
protected:
   // Location of the texture uniform
   GLint uTexture;

   // Location of the texture coordinate attribute
   GLint aTexCoord;

   // The texture ID
   GLuint textureID;

   // The texture unit
   GLenum textureUnit;

public:
   TextureMaterial(const ShaderProgram &shaderProgram, GLuint textureID, const std::string &textureUniformName);

   virtual ~TextureMaterial();

   virtual void apply(const Mesh &mesh);

   virtual void disable();

   void setTextureID(GLuint textureID);
};

#endif
