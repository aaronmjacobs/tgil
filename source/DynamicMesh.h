#ifndef DYNAMIC_MESH_H
#define DYNAMIC_MESH_H

#include "Mesh.h"

class DynamicMesh : public Mesh {
public:
   DynamicMesh();

   virtual ~DynamicMesh();

   void setVertices(float *vertices, unsigned int numVertices);

   void setNormals(float *normals, unsigned int numNormals);

   void setIndices(unsigned int *indices, unsigned int numIndices);

   void setTexCoords(float *texCoords, unsigned int numTexCoords);
};

#endif
