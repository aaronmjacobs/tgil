#ifndef MODEL_H
#define MODEL_H

#include "Types.h"

class Material;
class Mesh;

class Model {
protected:
   UPtr<Material> material;
   SPtr<Mesh> mesh;

public:
   Model(UPtr<Material> material, SPtr<Mesh> mesh);

   virtual ~Model();

   virtual void draw();

   const Material& getMaterial();

   const Mesh& getMesh();
};

#endif
