#ifndef MATERIAL_H
#define MATERIAL_H

#include "Types.h"

class Mesh;
class ShaderProgram;

class Material {
public:
   Material() {}

   virtual ~Material() {}

   /**
    * Applies the material properties to the given shader program with the given mesh
    */
   virtual void apply(const Mesh &mesh) = 0;
};

#endif
