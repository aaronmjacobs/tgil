#include "Shader.h"

Shader::Shader(const GLenum type)
   : id(glCreateShader(type)), type(type) {
}

Shader::~Shader() {
   glDeleteShader(id);
}

bool Shader::compile(const std::string &source) {
   // Set and compile the source
   const char *sourceData = source.c_str();
   glShaderSource(id, 1, &sourceData, NULL);
   glCompileShader(id);

   // Check if the shader compiled successfully
   GLint status;
   glGetShaderiv(id, GL_COMPILE_STATUS, &status);
   return status == GL_TRUE;
}
