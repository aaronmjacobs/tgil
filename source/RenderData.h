#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include "Types.h"

class ShaderProgram;

class RenderData {
protected:
   SPtr<ShaderProgram> overrideProgram;

public:
   RenderData();

   virtual ~RenderData();

   void setOverrideProgram(SPtr<ShaderProgram> overrideProgram);

   SPtr<ShaderProgram> getOverrideProgram() const;
};

#endif
