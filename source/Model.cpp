#include "Model.h"

#include "Material.h"
#include "Mesh.h"

#include <string>

Model::Model(UPtr<Material> material, SPtr<Mesh> mesh)
   : material(std::move(material)), mesh(mesh) {
}

Model::~Model() {
}

void Model::draw() {
   // Apply the material properties (and enable the shader)
   material->apply(*mesh);
   SPtr<ShaderProgram> shaderProgram = material->getShaderProgram();

   // Prepare the vertex buffer object
   glBindBuffer(GL_ARRAY_BUFFER, mesh->getVBO());
   GLint aPosition = shaderProgram->getAttribute("aPosition");
   glEnableVertexAttribArray(aPosition);
   glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Prepare the normal buffer object
   GLint aNormal = 0;
   bool hasNormals = shaderProgram->hasAttribute("aNormal");
   if (hasNormals) {
      glBindBuffer(GL_ARRAY_BUFFER, mesh->getNBO());
      aNormal = shaderProgram->getAttribute("aNormal");
      glEnableVertexAttribArray(aNormal);
      glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
   }

   // Prepare the index buffer object
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getIBO());

   // Draw
   glDrawElements(GL_TRIANGLES, mesh->getNumIndices(), GL_UNSIGNED_INT, 0);

   // Unbind
   glDisableVertexAttribArray(aPosition);
   if (hasNormals) {
      glDisableVertexAttribArray(aNormal);
   }
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   material->disable();
}

const Material& Model::getMaterial() {
   return *material;
}

const Mesh& Model::getMesh() {
   return *mesh;
}
