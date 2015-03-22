#include "Context.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "TextureMaterial.h"
#include "TextureUnitManager.h"

#include <string>

TextureMaterial::TextureMaterial(GLuint textureID, const std::string &textureUniformName, GLenum target)
   : textureID(textureID), target(target), textureUniformName(textureUniformName) {
}

TextureMaterial::~TextureMaterial() {
}

void TextureMaterial::apply(ShaderProgram &shaderProgram) {
   textureUnit = Context::getInstance().getTextureUnitManager().get();

   shaderProgram.setUniformValue(textureUniformName, textureUnit);

   glActiveTexture(GL_TEXTURE0 + textureUnit);
   glBindTexture(target, textureID);
}

void TextureMaterial::disable() {
   Context::getInstance().getTextureUnitManager().release(textureUnit);
}

void TextureMaterial::setTextureID(GLuint textureID) {
   this->textureID = textureID;
}
