#include "FancyAssert.h"
#include "Shader.h"
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram()
   : id(glCreateProgram()) {
}

ShaderProgram::~ShaderProgram() {
   glDeleteProgram(id);
}

void ShaderProgram::attach(SPtr<Shader> shader) {
   glAttachShader(id, shader->getID());
   shaders.push_back(shader);
}

bool ShaderProgram::link() {
   ASSERT(shaders.size() >= 2, "Need at least two shaders to link: %lu", shaders.size());

   glLinkProgram(id);

   // Check the status
   GLint linkStatus;
   glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
   return linkStatus == GL_TRUE;
}

void ShaderProgram::use() {
   glUseProgram(id);
}

void ShaderProgram::disable() {
   glUseProgram(0);
}

GLint ShaderProgram::addAttribute(const std::string &name) {
   const char *nameStr = name.c_str();
   GLint location = glGetAttribLocation(id, nameStr);
   ASSERT(location != -1, "Unable to get attribute location: %s", nameStr);

   attributeMap[name] = location;
   return location;
}

GLint ShaderProgram::addUniform(const std::string &name) {
   const char *nameStr = name.c_str();
   GLint location = glGetUniformLocation(id, nameStr);
   ASSERT(location != -1, "Unable to get uniform location: %s", nameStr);

   uniformMap[name] = location;
   return location;
}

GLint ShaderProgram::getAttribute(const std::string &name) {
   // Make sure the attribute exists in the map
   ASSERT(hasAttribute(name), "Unable to find attribute: %s", name.c_str());

   return attributeMap[name];
}

GLint ShaderProgram::getUniform(const std::string &name) {
   // Make sure the uniform exists in the map
   ASSERT(hasUniform(name), "Unable to find uniform: %s", name.c_str());

   return uniformMap[name];
}

bool ShaderProgram::hasAttribute(const std::string &name) {
   return attributeMap.count(name) > 0;
}

bool ShaderProgram::hasUniform(const std::string &name) {
   return uniformMap.count(name) > 0;
}
