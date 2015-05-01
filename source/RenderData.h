#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include "Types.h"

class ShaderProgram;

enum class RenderState {
   Color,
   Shadow
};

class RenderData {
protected:
   RenderState state;
   SPtr<ShaderProgram> overrideProgram;

public:
   RenderData(RenderState state = RenderState::Color);

   virtual ~RenderData();

   void setOverrideProgram(SPtr<ShaderProgram> overrideProgram);

   RenderState getRenderState() const;

   SPtr<ShaderProgram> getOverrideProgram() const;
};

#endif
