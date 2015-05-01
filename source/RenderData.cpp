#include "RenderData.h"

RenderData::RenderData(RenderState state)
   : state(state) {
}

RenderData::~RenderData() {
}

void RenderData::setOverrideProgram(SPtr<ShaderProgram> overrideProgram) {
   this->overrideProgram = overrideProgram;
}

RenderState RenderData::getRenderState() const {
   return state;
}

SPtr<ShaderProgram> RenderData::getOverrideProgram() const {
   return overrideProgram;
}
