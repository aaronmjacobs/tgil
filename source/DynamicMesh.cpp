#include "DynamicMesh.h"

DynamicMesh::DynamicMesh()
   : Mesh(nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0, GL_DYNAMIC_DRAW) {
   hasTextureBufferObject = true;
}

DynamicMesh::~DynamicMesh() {
}

void DynamicMesh::setVertices(float *vertices, unsigned int numVertices) {
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVertices, vertices, GL_DYNAMIC_DRAW);
}

void DynamicMesh::setNormals(float *normals, unsigned int numNormals) {
   glBindBuffer(GL_ARRAY_BUFFER, nbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numNormals, normals, GL_DYNAMIC_DRAW);
}

void DynamicMesh::setIndices(unsigned int *indices, unsigned int numIndices) {
   this->numIndices = numIndices;
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indices, GL_DYNAMIC_DRAW);
}

void DynamicMesh::setTexCoords(float *texCoords, unsigned int numTexCoords) {
   glBindBuffer(GL_ARRAY_BUFFER, tbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numTexCoords, texCoords, GL_DYNAMIC_DRAW);
}
