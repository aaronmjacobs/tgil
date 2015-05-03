#ifndef MESH_H
#define MESH_H

#include "GLIncludes.h"
#include "Types.h"

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
    * Packed array of vertices
    */
   UPtr<float[]> vertices;

   /**
    * Packed array of indices
    */
   UPtr<unsigned int[]> indices;

   /**
    * Number of vertices
    */
   unsigned int numVertices;

   /**
    * Number of indices
    */
   unsigned int numIndices;

   /**
    * If the mesh has a texture buffer object
    */
   bool hasTextureBufferObject;

public:
   Mesh(UPtr<float[]> vertices, unsigned int numVertices, UPtr<float[]> normals, unsigned int numNormals,
        UPtr<unsigned int[]> indices, unsigned int numIndices, UPtr<float[]> texCoords, unsigned int numTexCoords,
        GLenum usage = GL_STATIC_DRAW);

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

   unsigned int* getIndices() const {
      return indices.get();
   }

   float* getVertices() const {
      return vertices.get();
   }

   unsigned int getNumVertices() const {
      return numVertices;
   }
};

#endif
