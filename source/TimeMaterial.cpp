#include "Context.h"
#include "ShaderProgram.h"
#include "TimeMaterial.h"

TimeMaterial::TimeMaterial(const ShaderProgram &shaderProgram) {
   uTime = shaderProgram.getUniform("uTime");
}

TimeMaterial::~TimeMaterial() {
}

void TimeMaterial::apply(const Mesh &mesh) {
   float time = Context::getInstance().getRunningTime();
   glUniform1f(uTime, time);
}

void TimeMaterial::disable() {
}
