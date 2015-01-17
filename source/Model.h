#ifndef MODEL_H
#define MODEL_H

#include "Types.h"

#include <vector>

class Material;
class Mesh;
class ShaderProgram;

class Model {
protected:
   std::vector<UPtr<Material>> materials;
   SPtr<Mesh> mesh;
   SPtr<ShaderProgram> shaderProgram;

public:
   Model(SPtr<ShaderProgram> shaderProgram, UPtr<Material> material, SPtr<Mesh> mesh);

   virtual ~Model();

   virtual void draw();

   void attachMaterial(UPtr<Material> material);

   const Mesh& getMesh() const;

   const SPtr<ShaderProgram> getShaderProgram() const;
};

#endif
