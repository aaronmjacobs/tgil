#include "RenderData.h"

RenderData::RenderData(RenderState state)
   : state(state), overrideProgram(nullptr), renderingCameraObject(false) {
}

RenderData::~RenderData() {
}

void RenderData::setOverrideProgram(SPtr<ShaderProgram> overrideProgram) {
   this->overrideProgram = overrideProgram;
}

void RenderData::setRenderingCameraObject(bool renderingCameraObject) {
   this->renderingCameraObject = renderingCameraObject;
}

RenderState RenderData::getRenderState() const {
   return state;
}

SPtr<ShaderProgram> RenderData::getOverrideProgram() const {
   return overrideProgram;
}
