#include "Context.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "TextureMaterial.h"
#include "TextureUnitManager.h"

#include <string>

TextureMaterial::TextureMaterial(const ShaderProgram &shaderProgram, GLuint textureID, const std::string &textureUniformName, GLenum target)
   : textureID(textureID), target(target) {
   uTexture = shaderProgram.getUniform(textureUniformName);
}

TextureMaterial::~TextureMaterial() {
}

void TextureMaterial::apply(const Mesh &mesh) {
   textureUnit = Context::getInstance().getTextureUnitManager().get();

   glUniform1i(uTexture, textureUnit);
   glActiveTexture(GL_TEXTURE0 + textureUnit);
   glBindTexture(target, textureID);

   glBindBuffer(GL_ARRAY_BUFFER, mesh.getTBO());
   glEnableVertexAttribArray(ShaderAttributes::TEX_COORD);
   glVertexAttribPointer(ShaderAttributes::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void TextureMaterial::disable() {
   glDisableVertexAttribArray(ShaderAttributes::TEX_COORD);

   Context::getInstance().getTextureUnitManager().release(textureUnit);
}

void TextureMaterial::setTextureID(GLuint textureID) {
   this->textureID = textureID;
}
