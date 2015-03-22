#ifndef TEXTURE_MATERIAL_H
#define TEXTURE_MATERIAL_H

#include "GLIncludes.h"
#include "Material.h"

class ShaderProgram;

class TextureMaterial : public Material {
protected:
   // The texture ID
   GLuint textureID;

   // The texture unit
   GLenum textureUnit;

   // The texture target
   GLenum target;

   // Name of the texture uniform
   std::string textureUniformName;

public:
   TextureMaterial(GLuint textureID, const std::string &textureUniformName, GLenum target = GL_TEXTURE_2D);

   virtual ~TextureMaterial();

   virtual void apply(ShaderProgram &shaderProgram);

   virtual void disable();

   void setTextureID(GLuint textureID);
};

#endif
