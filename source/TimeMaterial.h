#ifndef TIME_MATERIAL_H
#define TIME_MATERIAL_H

#include "GLIncludes.h"
#include "Material.h"

class ShaderProgram;

class TimeMaterial : public Material {
protected:
   // Location of the time uniform
   GLint uTime;

public:
   TimeMaterial(const ShaderProgram &shaderProgram);

   virtual ~TimeMaterial();

   virtual void apply(const Mesh &mesh);

   virtual void disable();
};

#endif
