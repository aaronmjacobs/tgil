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

DebugRenderer::DebugRenderer()
   : shaderProgram(new ShaderProgram) {
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &cbo);
   glGenBuffers(1, &ibo);
}

DebugRenderer::~DebugRenderer() {
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &cbo);
   glDeleteBuffers(1, &ibo);
}

void DebugRenderer::init() {
   const Context &context = Context::getInstance();
   SPtr<Shader> vertexShader = context.getAssetManager().loadShader("shaders/debug_vert.glsl", GL_VERTEX_SHADER);
   SPtr<Shader> fragmentShader = context.getAssetManager().loadShader("shaders/debug_frag.glsl", GL_FRAGMENT_SHADER);

   shaderProgram->attach(vertexShader);
   shaderProgram->attach(fragmentShader);
   bool linked = shaderProgram->link();
   if (!linked) {
      LOG_WARNING("Unable to link debug draw shader");
   }

   shaderProgram->addUniform("uViewMatrix");
   shaderProgram->addUniform("uProjMatrix");

   shaderProgram->addAttribute("aPosition");
   shaderProgram->addAttribute("aColor");
}

void DebugRenderer::render(const DebugDrawer &drawer, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
   shaderProgram->use();

   // View matrix
   GLint uViewMatrix = shaderProgram->getUniform("uViewMatrix");
   glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

   // Projection matrix
   GLint uProjMatrix = shaderProgram->getUniform("uProjMatrix");
   glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

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

   // Prepare the vertex buffer object
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   GLint aPosition = shaderProgram->getAttribute("aPosition");
   glEnableVertexAttribArray(aPosition);
   glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Prepare the color buffer object
   glBindBuffer(GL_ARRAY_BUFFER, cbo);
   GLint aColor = shaderProgram->getAttribute("aColor");
   glEnableVertexAttribArray(aColor);
   glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Prepare the index buffer object
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

   // Draw
   glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

   // Unbind
   glDisableVertexAttribArray(aPosition);
   glDisableVertexAttribArray(aColor);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   shaderProgram->disable();
}
