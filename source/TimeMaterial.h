#ifndef TIME_MATERIAL_H
#define TIME_MATERIAL_H

#include "GLIncludes.h"
#include "Material.h"

class ShaderProgram;

class TimeMaterial : public Material {
public:
   TimeMaterial();

   virtual ~TimeMaterial();

   virtual void apply(ShaderProgram &shaderProgram);

   virtual void disable();
};

#endif
