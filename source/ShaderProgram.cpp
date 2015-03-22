#include "Context.h"
#include "FancyAssert.h"
#include "LogHelper.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

// Uniform

Uniform::Uniform(const GLint location, const GLenum type, const std::string &name)
   : location(location), type(type), name(name), dirty(false) {
}

Uniform::~Uniform() {
}

void Uniform::commit() {
   if (!dirty) {
      return;
   }

   switch (type) {
      case GL_BOOL:
         glUniform1i(location, pendingData.boolVal);
         break;
      case GL_INT:
      case GL_SAMPLER_1D:
      case GL_SAMPLER_2D:
      case GL_SAMPLER_3D:
      case GL_SAMPLER_CUBE:
      case GL_SAMPLER_1D_SHADOW:
      case GL_SAMPLER_2D_SHADOW:
      case GL_SAMPLER_CUBE_SHADOW:
         glUniform1i(location, pendingData.intVal);
         break;
      case GL_FLOAT:
         glUniform1f(location, pendingData.floatVal);
         break;
      case GL_FLOAT_VEC2:
         glUniform2fv(location, 1, glm::value_ptr(pendingData.vec2Val));
         break;
      case GL_FLOAT_VEC3:
         glUniform3fv(location, 1, glm::value_ptr(pendingData.vec3Val));
         break;
      case GL_FLOAT_VEC4:
         glUniform4fv(location, 1, glm::value_ptr(pendingData.vec4Val));
         break;
      case GL_FLOAT_MAT4:
         glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(pendingData.mat4Val));
         break;
      default:
         ASSERT(false, "Invalid uniform type: %d", type);
         break;
   }

   activeData = pendingData;
   dirty = false;
}

void Uniform::setValue(bool value) {
   ASSERT(type == GL_BOOL);
   if (pendingData.boolVal != value) {
      dirty = true;
      pendingData.boolVal = value;
   }
}

void Uniform::setValue(int value) {
   ASSERT(type == GL_INT ||
          type == GL_SAMPLER_1D ||
          type == GL_SAMPLER_2D ||
          type == GL_SAMPLER_3D ||
          type == GL_SAMPLER_CUBE ||
          type == GL_SAMPLER_1D_SHADOW ||
          type == GL_SAMPLER_2D_SHADOW ||
          type == GL_SAMPLER_CUBE_SHADOW);
   if (pendingData.intVal != value) {
      dirty = true;
      pendingData.intVal = value;
   }
}

void Uniform::setValue(GLenum value) {
   setValue((int)value);
}

void Uniform::setValue(float value) {
   ASSERT(type == GL_FLOAT);
   if (pendingData.floatVal != value) {
      dirty = true;
      pendingData.floatVal = value;
   }
}

void Uniform::setValue(const glm::vec2 &value) {
   ASSERT(type == GL_FLOAT_VEC2);
   if (pendingData.vec2Val != value) {
      dirty = true;
      pendingData.vec2Val = value;
   }
}

void Uniform::setValue(const glm::vec3 &value) {
   ASSERT(type == GL_FLOAT_VEC3);
   if (pendingData.vec3Val != value) {
      dirty = true;
      pendingData.vec3Val = value;
   }
}

void Uniform::setValue(const glm::vec4 &value) {
   ASSERT(type == GL_FLOAT_VEC4);
   if (pendingData.vec4Val != value) {
      dirty = true;
      pendingData.vec4Val = value;
   }
}

void Uniform::setValue(const glm::mat4 &value) {
   ASSERT(type == GL_FLOAT_MAT4);
   if (pendingData.mat4Val != value) {
      dirty = true;
      pendingData.mat4Val = value;
   }
}

// ShaderProgram

ShaderProgram::ShaderProgram()
   : id(glCreateProgram()), context(Context::getInstance()) {
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

   uniforms.clear();

   glLinkProgram(id);

   // Check the status
   GLint linkStatus;
   glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
   if (linkStatus != GL_TRUE) {
      return false;
   }

   GLint numUniforms;
   glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numUniforms);

   const int nameBufSize = 512;
   char nameBuf[nameBufSize];
   for (int i = 0; i < numUniforms; ++i) {
      GLsizei length = 0;
      GLint size = 0;
      GLenum type;
      glGetActiveUniform(id, i, nameBufSize, &length, &size, &type, nameBuf);

      if (length < 1 || size < 1) {
         LOG_WARNING("Unable to get active uniform: " << i);
      } else {
         std::string name(nameBuf);
         GLint location = glGetUniformLocation(id, name.c_str());
         uniforms[name] = std::make_shared<Uniform>(location, type, name);
      }
   }

   return true;
}

void ShaderProgram::use() const {
   if (id != context.getActiveShaderProgramID()) {
      glUseProgram(id);
      context.setActiveShaderProgramID(id);
   }
}

bool ShaderProgram::hasUniform(const std::string &name) const {
   return uniforms.count(name) > 0;
}

SPtr<Uniform> ShaderProgram::getUniform(const std::string &name) const {
   ASSERT(hasUniform(name), "Uniform with given name doesn't exist: %s", name.c_str());
   return uniforms.at(name);
}

void ShaderProgram::commit() {
   use();

   for (UniformMap::iterator itr = uniforms.begin(); itr != uniforms.end(); ++itr) {
      itr->second->commit();
   }
}
