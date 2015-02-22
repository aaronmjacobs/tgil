#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "GLIncludes.h"

#include <glm/glm.hpp>

class DebugDrawer;
class ShaderProgram;

class DebugRenderer {
protected:
   /**
    * Vertex buffer object
    */
   GLuint vbo;

   /**
    * Color buffer object
    */
   GLuint cbo;

   /**
    * Index buffer object
    */
   GLuint ibo;

   /**
    * Vertex array object
    */
   GLuint vao;

   SPtr<ShaderProgram> shaderProgram;

public:
   DebugRenderer();

   virtual ~DebugRenderer();

   void init();

   void render(const DebugDrawer &drawer, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

   const ShaderProgram& getShaderProgram() const {
      return *shaderProgram;
   }
};

#endif
