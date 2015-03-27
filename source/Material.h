#ifndef MATERIAL_H
#define MATERIAL_H

#include "Types.h"

class ShaderProgram;

class Material {
public:
   Material() {}

   virtual ~Material() {}

   /**
    * Applies the material properties to the given shader program
    */
   virtual void apply(ShaderProgram &shaderProgram) = 0;

   /**
    * Disables any changed states that were set in apply
    */
   virtual void disable() = 0;
};

#endif
