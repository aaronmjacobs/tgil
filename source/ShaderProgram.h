#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "GLIncludes.h"
#include "Types.h"

#include <string>
#include <unordered_map>
#include <vector>

class Shader;

namespace ShaderAttributes {
   enum Attributes : GLint {
      POSITION = 0,
      NORMAL = 1,
      TEX_COORD = 2,
      COLOR = 3,
   };
} // namespace ShaderAttributes

typedef std::unordered_map<std::string, GLint> UniformMap;

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
    * A map of uniform names to their location
    */
   UniformMap uniformMap;

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
    * Sets the program as the active program
    */
   void use() const;

   /**
    * Disables the program
    */
   void disable() const;

   /**
    * Adds the uniform with the given name to the uniform map
    */
   GLint addUniform(const std::string &name);

   /**
    * Gets the location of the uniform with the given name from the uniform map
    */
   GLint getUniform(const std::string &name) const;

   /**
    * Returns whether the program has a uniform with the given name
    */
   bool hasUniform(const std::string &name) const;
};

#endif
