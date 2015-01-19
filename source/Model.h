#ifndef MODEL_H
#define MODEL_H

#include "Types.h"

#include <vector>

class Material;
class Mesh;
class ShaderProgram;

class Model {
protected:
   std::vector<SPtr<Material>> materials;
   SPtr<ShaderProgram> shaderProgram;
   SPtr<Mesh> mesh;

public:
   Model(SPtr<ShaderProgram> shaderProgram, SPtr<Material> material, SPtr<Mesh> mesh);

   virtual ~Model();

   virtual void draw();

   void attachMaterial(SPtr<Material> material);

   const Mesh& getMesh() const;

   const SPtr<ShaderProgram> getShaderProgram() const;
};

#endif
