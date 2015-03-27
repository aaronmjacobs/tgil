#include "AssetManager.h"
#include "Context.h"
#include "DebugDrawer.h"
#include "DebugRenderer.h"
#include "LogHelper.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

namespace {

const float DEBUG_POINT_SIZE = 10.0f;

} // namespace

DebugRenderer::DebugRenderer() {
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &cbo);
   glGenBuffers(1, &ibo);
   glGenVertexArrays(1, &vao);
}

DebugRenderer::~DebugRenderer() {
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &cbo);
   glDeleteBuffers(1, &ibo);
   glDeleteVertexArrays(1, &vao);
}

void DebugRenderer::init() {
   const Context &context = Context::getInstance();
   shaderProgram = context.getAssetManager().loadShaderProgram("shaders/debug");

   glBindVertexArray(vao);

   // Prepare the vertex buffer object
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glEnableVertexAttribArray(ShaderAttributes::POSITION);
   glVertexAttribPointer(ShaderAttributes::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Prepare the color buffer object
   glBindBuffer(GL_ARRAY_BUFFER, cbo);
   glEnableVertexAttribArray(ShaderAttributes::COLOR);
   glVertexAttribPointer(ShaderAttributes::COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Prepare the index buffer object
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

   glBindVertexArray(0);
}

void DebugRenderer::render(const DebugDrawer &drawer, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
   // View matrix
   shaderProgram->setUniformValue("uViewMatrix", viewMatrix);

   // Projection matrix
   shaderProgram->setUniformValue("uProjMatrix", projectionMatrix);

   shaderProgram->commit();

   const std::vector<Line> &lines = drawer.getLines();
   const std::vector<LineColor> &colors = drawer.getColors();

   // Generate indices
   std::vector<unsigned int> indices(lines.size() * 2);
   for (unsigned int i = 0; i < indices.size(); ++i) {
      indices[i] = i;
   }

   // Pass the vertices / colors / indices to the GPU
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * lines.size(), lines.data(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, cbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(LineColor) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

   // Bind
   glBindVertexArray(vao);

   // Draw
   glPointSize(DEBUG_POINT_SIZE);
   glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
   glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

   // Unbind
   glBindVertexArray(0);
}
