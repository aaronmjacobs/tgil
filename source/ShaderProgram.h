#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "GLIncludes.h"
#include "LogHelper.h"
#include "Types.h"

#include <glm/glm.hpp>

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

class Context;
class Shader;

namespace ShaderAttributes {

enum Attributes : GLint {
   POSITION = 0,
   NORMAL = 1,
   TEX_COORD = 2,
   COLOR = 3,
};

} // namespace ShaderAttributes

union UniformData {
   bool boolVal;
   int intVal;
   float floatVal;
   glm::vec2 vec2Val;
   glm::vec3 vec3Val;
   glm::vec4 vec4Val;
   glm::mat4 mat4Val;

   UniformData() {
      memset(this, 0, sizeof(UniformData));
   }

   ~UniformData() {
   }
};

class Uniform {
protected:
   const GLint location;
   const GLenum type;
   const std::string name;
   UniformData activeData;
   UniformData pendingData;
   bool dirty;

public:
   Uniform(const GLint location, const GLenum type, const std::string &name);

   virtual ~Uniform();

   void commit();

   GLenum getType() const {
      return type;
   }

   const std::string& getName() const {
      return name;
   }

   const UniformData& getActiveData() const {
      return activeData;
   }

   UniformData& getPendingData() {
      dirty = true;
      return pendingData;
   }

   const UniformData& getPendingData() const {
      return pendingData;
   }

   void setValue(bool value);

   void setValue(int value);

   void setValue(GLenum value);

   void setValue(float value);

   void setValue(const glm::vec2 &value);

   void setValue(const glm::vec3 &value);

   void setValue(const glm::vec4 &value);

   void setValue(const glm::mat4 &value);
};

typedef std::unordered_map<std::string, SPtr<Uniform>> UniformMap;

class ShaderProgram {
protected:
   /**
    * The shader program's handle
    */
   const GLuint id;

   /**
    * All shaders attached to the program
    */
   std::vector<SPtr<Shader>> shaders;

   /**
    * All uniforms in the shader program
    */
   UniformMap uniforms;

   Context &context;

   /**
    * Sets the program as the active program
    */
   void use() const;

public:
   ShaderProgram();

   virtual ~ShaderProgram();

   /**
    * Gets the shader program's handle
    */
   GLuint getID() const {
      return id;
   }

   /**
    * Attaches the given shader to the program
    */
   void attach(SPtr<Shader> shader);

   /**
    * Links the shader program
    */
   bool link();

   /**
    * Returns whether the program has a uniform with the given name
    */
   bool hasUniform(const std::string &name) const;

   /**
    * Gets the uniform with the given name
    */
   SPtr<Uniform> getUniform(const std::string &name) const;

   /**
    * Commits the values of all uniforms in the shader program
    */
   void commit();

   /**
    * Sets the value of the uniform with the given name
    */
   template<typename T>
   void setUniformValue(const std::string &name, const T &value, bool ignoreFailure = false) {
      UniformMap::iterator itr = uniforms.find(name);
      if (itr == uniforms.end()) {
         if (!ignoreFailure) {
            LOG_WARNING("Uniform with given name doesn't exist: " << name);
         }

         return;
      }

      itr->second->setValue(value);
   }
};

#endif
