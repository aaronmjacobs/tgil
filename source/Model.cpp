#include "Model.h"

#include "Material.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <string>

Model::Model(SPtr<ShaderProgram> shaderProgram, SPtr<Mesh> mesh)
   : shaderProgram(shaderProgram), mesh(mesh) {
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   // Prepare the vertex buffer object
   glBindBuffer(GL_ARRAY_BUFFER, mesh->getVBO());
   glEnableVertexAttribArray(ShaderAttributes::POSITION);
   glVertexAttribPointer(ShaderAttributes::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Prepare the normal buffer object
   glBindBuffer(GL_ARRAY_BUFFER, mesh->getNBO());
   glEnableVertexAttribArray(ShaderAttributes::NORMAL);
   glVertexAttribPointer(ShaderAttributes::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Prepare the index buffer object
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getIBO());

   glBindVertexArray(0);
}

Model::~Model() {
   glDeleteVertexArrays(1, &vao);
}

void Model::draw(const RenderData &renderData) {
   shaderProgram->use();

   // Bind
   glBindVertexArray(vao);

   // Apply the material properties
   for (SPtr<Material> material : materials) {
      material->apply(*mesh);
   }

   // Draw
   glDrawElements(GL_TRIANGLES, mesh->getNumIndices(), GL_UNSIGNED_INT, 0);

   // Disable the material properties
   for (SPtr<Material> material : materials) {
      material->disable();
   }

   // Unbind
   glBindVertexArray(0);

   shaderProgram->disable();
}

void Model::attachMaterial(SPtr<Material> material) {
   materials.push_back(material);
}

void Model::clearMaterials() {
   materials.clear();
}

const Mesh& Model::getMesh() const {
   return *mesh;
}

const SPtr<ShaderProgram> Model::getShaderProgram() const {
   return shaderProgram;
}
