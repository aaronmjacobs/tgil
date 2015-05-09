#include "Context.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "TextureMaterial.h"
#include "TextureUnitManager.h"

#include <string>

TextureMaterial::TextureMaterial(SPtr<Texture> texture, const std::string &textureUniformName)
   : texture(texture), textureUniformName(textureUniformName) {
}

TextureMaterial::~TextureMaterial() {
}

void TextureMaterial::apply(ShaderProgram &shaderProgram) {
   if (!texture) {
      return;
   }

   textureUnit = Context::getInstance().getTextureUnitManager().get();

   shaderProgram.setUniformValue(textureUniformName, textureUnit);

   glActiveTexture(GL_TEXTURE0 + textureUnit);
   texture->bind();
}

void TextureMaterial::disable() {
   Context::getInstance().getTextureUnitManager().release(textureUnit);
}

void TextureMaterial::setTexture(SPtr<Texture> texture) {
   this->texture = texture;
}
