#ifndef TEXTURE_MATERIAL_H
#define TEXTURE_MATERIAL_H

#include "GLIncludes.h"
#include "Material.h"

class ShaderProgram;

class TextureMaterial : public Material {
protected:
   // Location of the texture uniform
   GLint uTexture;

   // The texture ID
   GLuint textureID;

   // The texture unit
   GLenum textureUnit;

   // The texture target
   GLenum target;

public:
   TextureMaterial(const ShaderProgram &shaderProgram, GLuint textureID, const std::string &textureUniformName, GLenum target = GL_TEXTURE_2D);

   virtual ~TextureMaterial();

   virtual void apply(const Mesh &mesh);

   virtual void disable();

   void setTextureID(GLuint textureID);
};

#endif
