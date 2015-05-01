#include "ShaderProgram.h"
#include "TintMaterial.h"

TintMaterial::TintMaterial(float opacity, const glm::vec3 &tint)
   : opacity(opacity), tint(tint) {
}

TintMaterial::~TintMaterial() {
}

void TintMaterial::apply(ShaderProgram &shaderProgram) {
   shaderProgram.setUniformValue("uOpacity", opacity);
   shaderProgram.setUniformValue("uTint", tint);
}

void TintMaterial::disable() {
}

void TintMaterial::setValues(float opacity, const glm::vec3 &tint) {
   this->opacity = opacity;
   this->tint = tint;
}
