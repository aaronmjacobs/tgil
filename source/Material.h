#ifndef MATERIAL_H
#define MATERIAL_H

#include "Types.h"

class Mesh;

class Material {
public:
   Material() {}

   virtual ~Material() {}

   /**
    * Applies the material properties to the given shader program with the given mesh
    */
   virtual void apply(const Mesh &mesh) = 0;

   /**
    * Disables any changed states that were set in apply
    */
   virtual void disable() = 0;
};

#endif
