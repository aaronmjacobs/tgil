#ifndef MESH_H
#define MESH_H

#include "GLIncludes.h"
#include "Types.h"

struct aiMesh;

class Mesh {
protected:
   /**
    * Vertex buffer object
    */
   GLuint vbo;

   /**
    * Normal buffer object
    */
   GLuint nbo;

   /**
    * Index buffer object
    */
   GLuint ibo;

   /**
    * Texture buffer object
    */
   GLuint tbo;

   /**
    * If the mesh has a texture buffer object
    */
   bool hasTextureBufferObject;

   /**
    * Number of vertex indices
    */
   unsigned int numIndices;

   /**
    * Packed array of vertices
    */
   UPtr<float[]> vertices;

   /**
    * Number of vertices
    */
   unsigned int numVertices;

public:
   Mesh(const aiMesh* aiMesh);

   virtual ~Mesh();

   GLuint getVBO() const {
     return vbo;
   }

   GLuint getNBO() const {
     return nbo;
   }

   GLuint getIBO() const {
     return ibo;
   }

   bool hasTBO() const {
      return hasTextureBufferObject;
   }

   GLuint getTBO() const;

   unsigned int getNumIndices() const {
     return numIndices;
   }

   float* getVertices() const {
      return vertices.get();
   }

   unsigned int getNumVertices() const {
      return numVertices;
   }
};

#endif
