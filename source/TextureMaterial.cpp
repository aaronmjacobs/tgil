#include "Context.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "TextureMaterial.h"
#include "TextureUnitManager.h"

#include <string>

TextureMaterial::TextureMaterial(const ShaderProgram &shaderProgram, GLuint textureID, const std::string &textureUniformName, GLenum target)
   : textureID(textureID), target(target) {
   uTexture = shaderProgram.getUniform(textureUniformName);

   hasTexCoord = shaderProgram.hasAttribute("aTexCoord");
   if (hasTexCoord) {
      aTexCoord = shaderProgram.getAttribute("aTexCoord");
   }
}

TextureMaterial::~TextureMaterial() {
}

void TextureMaterial::apply(const Mesh &mesh) {
   textureUnit = Context::getInstance().getTextureUnitManager().get();

   glUniform1i(uTexture, textureUnit);
   glActiveTexture(GL_TEXTURE0 + textureUnit);
   glBindTexture(target, textureID);

   if (hasTexCoord) {
      glBindBuffer(GL_ARRAY_BUFFER, mesh.getTBO());
      glEnableVertexAttribArray(aTexCoord);
      glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
   }
}

void TextureMaterial::disable() {
   if (hasTexCoord) {
      glDisableVertexAttribArray(aTexCoord);
   }

   Context::getInstance().getTextureUnitManager().release(textureUnit);
}

void TextureMaterial::setTextureID(GLuint textureID) {
   this->textureID = textureID;
}
