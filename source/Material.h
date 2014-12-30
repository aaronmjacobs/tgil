#ifndef MATERIAL_H
#define MATERIAL_H

#include "ShaderProgram.h"
#include "Types.h"

class Mesh;

class Material {
protected:
   /**
    * Shader program used to draw
    */
   SPtr<ShaderProgram> shaderProgram;

public:
   Material(SPtr<ShaderProgram> program)
      : shaderProgram(program) {}

   virtual ~Material() {}

   /**
    * Applies the material properties to the shader with the given mesh
    */
   virtual void apply(const Mesh &mesh) = 0;

   /**
    * Disables the material
    */
   virtual void disable() = 0;

   const ShaderProgram& getShaderProgram() const {
      return *shaderProgram;
   }
};

#endif
