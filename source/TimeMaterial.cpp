#include "Context.h"
#include "ShaderProgram.h"
#include "TimeMaterial.h"

TimeMaterial::TimeMaterial() {
}

TimeMaterial::~TimeMaterial() {
}

void TimeMaterial::apply(ShaderProgram &shaderProgram) {
   float time = Context::getInstance().getRunningTime();
   shaderProgram.setUniformValue("uTime", time);
}

void TimeMaterial::disable() {
}
