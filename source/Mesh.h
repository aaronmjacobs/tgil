#ifndef MESH_H
#define MESH_H

#include "GLIncludes.h"

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

   GLuint getTBO() const;

   unsigned int getNumIndices() const {
     return numIndices;
   }
};

#endif
