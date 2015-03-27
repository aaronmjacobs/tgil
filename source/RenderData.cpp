#include "RenderData.h"

RenderData::RenderData() {
}

RenderData::~RenderData() {
}

void RenderData::setOverrideProgram(SPtr<ShaderProgram> overrideProgram) {
   this->overrideProgram = overrideProgram;
}

SPtr<ShaderProgram> RenderData::getOverrideProgram() const {
   return overrideProgram;
}
