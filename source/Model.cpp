#include "Model.h"

#include "Material.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <string>

Model::Model(SPtr<ShaderProgram> shaderProgram, UPtr<Material> material, SPtr<Mesh> mesh)
   : shaderProgram(shaderProgram), mesh(mesh) {
   materials.push_back(std::move(material));
}

Model::~Model() {
}

void Model::draw() {
   shaderProgram->use();

   // Apply the material properties
   for (UPtr<Material> &material : materials) {
      material->apply(*mesh);
   }

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

   shaderProgram->disable();
}

void Model::attachMaterial(UPtr<Material> material) {
   materials.push_back(std::move(material));
}

const Mesh& Model::getMesh() const {
   return *mesh;
}

const SPtr<ShaderProgram> Model::getShaderProgram() const {
   return shaderProgram;
}
