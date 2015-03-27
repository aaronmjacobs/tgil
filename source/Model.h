#ifndef MODEL_H
#define MODEL_H

#include "GLIncludes.h"
#include "Types.h"

#include <vector>

class Material;
class Mesh;
class RenderData;
class ShaderProgram;

class Model {
protected:
   std::vector<SPtr<Material>> materials;
   SPtr<ShaderProgram> shaderProgram;
   SPtr<Mesh> mesh;

   // Vertex array object
   GLuint vao;

public:
   Model(SPtr<ShaderProgram> shaderProgram, SPtr<Mesh> mesh);

   virtual ~Model();

   virtual void draw(const RenderData &renderData);

   void attachMaterial(SPtr<Material> material);

   void clearMaterials();

   const Mesh& getMesh() const;

   const SPtr<ShaderProgram> getShaderProgram() const;
};

#endif
