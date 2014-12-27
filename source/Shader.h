#ifndef SHADER_H
#define SHADER_H

#include "GLIncludes.h"

#include <string>

class Shader {
protected:
   /**
    * The shader's handle
    */
   const GLuint id;

   /**
    * The type of shader (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, or
    * GL_GEOMETRY_SHADER)
    */
   const GLenum type;

public:
   /**
    * Constructs the shader with the given type
    */
   Shader(const GLenum type);

   virtual ~Shader();

   /**
    * Compiles the shader with the given source
    */
   bool compile(const std::string &source);

   /**
    * Gets the shader's handle
    */
   GLuint getID() const {
      return id;
   }

   /**
    * Gets the shader's type
    */
   GLenum getType() const {
      return type;
   }
};

#endif
