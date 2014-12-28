#include "FancyAssert.h"
#include "Mesh.h"

#include <assimp/scene.h>

Mesh::Mesh(const aiMesh* aiMesh) {
   ASSERT(aiMesh, "Given null aiMesh");

   // Parse the faces
   numIndices = aiMesh->mNumFaces * 3;
   unsigned int *faceArray = new unsigned int[numIndices];
   unsigned int faceIndex = 0;

   for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
      const aiFace* face = &aiMesh->mFaces[t];

      memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
      faceIndex += 3;
   }

   // Prepare the vertex buffer object
   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * aiMesh->mNumVertices, aiMesh->mVertices, GL_STATIC_DRAW);

   // Prepare the normal buffer object
   glGenBuffers(1, &nbo);
   glBindBuffer(GL_ARRAY_BUFFER, nbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * aiMesh->mNumVertices, aiMesh->mNormals, GL_STATIC_DRAW);

   // Prepare the index buffer object
   glGenBuffers(1, &ibo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, faceArray, GL_STATIC_DRAW);
   delete[] faceArray;

   // Buffer for vertex texture coordinates
   if (aiMesh->HasTextureCoords(0)) {
      float *texCoords = new float[aiMesh->mNumVertices * 2];
      for (unsigned int k = 0; k < aiMesh->mNumVertices; ++k) {
         texCoords[k * 2] = aiMesh->mTextureCoords[0][k].x;
         texCoords[k * 2 + 1] = 1.0f - aiMesh->mTextureCoords[0][k].y;
      }

      glGenBuffers(1, &tbo);
      glBindBuffer(GL_ARRAY_BUFFER, tbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * aiMesh->mNumVertices, texCoords, GL_STATIC_DRAW);

      hasTextureBufferObject = true;
      delete[] texCoords;
   } else {
      hasTextureBufferObject = false;
      tbo = 0;
   }

   // Unbind
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
