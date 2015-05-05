#include "FancyAssert.h"
#include "Mesh.h"

Mesh::Mesh(UPtr<float[]> vertices, unsigned int numVertices, UPtr<float[]> normals, unsigned int numNormals,
           UPtr<unsigned int[]> indices, unsigned int numIndices, UPtr<float[]> texCoords, unsigned int numTexCoords,
           GLenum usage) {
   ASSERT(numVertices == 0 || vertices, "numVertices > 0, but no vertices provided");
   ASSERT(numNormals == 0 || normals, "numNormals > 0, but no normals provided");
   ASSERT(numIndices == 0 || indices, "numIndices > 0, but no indices provided");
   ASSERT(numTexCoords == 0 || texCoords, "numTexCoords > 0, but no texCoords provided");
   ASSERT(usage == GL_STATIC_DRAW || usage == GL_DYNAMIC_DRAW, "Invalid usage: %u", usage);

   // Prepare the vertex buffer object
   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVertices, vertices.get(), usage);

   // Prepare the normal buffer object
   glGenBuffers(1, &nbo);
   glBindBuffer(GL_ARRAY_BUFFER, nbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numNormals, normals.get(), usage);

   // Prepare the index buffer object
   glGenBuffers(1, &ibo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indices.get(), usage);

   // Buffer for vertex texture coordinates
   glGenBuffers(1, &tbo);
   glBindBuffer(GL_ARRAY_BUFFER, tbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numTexCoords, texCoords.get(), usage);

   // Unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   this->vertices = std::move(vertices);
   this->numVertices = numVertices;
   this->indices = std::move(indices);
   this->numIndices = numIndices;
   hasTextureBufferObject = numTexCoords > 0;
}

Mesh::~Mesh() {
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &nbo);
   glDeleteBuffers(1, &ibo);
   if (hasTextureBufferObject) {
      glDeleteBuffers(1, &tbo);
   }
}

GLuint Mesh::getTBO() const {
   ASSERT(hasTextureBufferObject, "Mesh doesn't have texture coordinates");
   return tbo;
}
