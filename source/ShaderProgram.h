#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "GLIncludes.h"
#include "Types.h"

#include <map>
#include <vector>

class Shader;

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
    * A map of attribute names to their locations
    */
   std::map<std::string, GLint> attributeMap;

   /**
    * A map of uniform names to their location
    */
   std::map<std::string, GLint> uniformMap;

public:
   ShaderProgram();

   virtual ~ShaderProgram();

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
   void use();

   /**
    * Disables the program
    */
   void disable();

   /**
    * Adds the attribute with the given name to the attribute map
    */
   GLint addAttribute(const std::string &name);

   /**
    * Adds the uniform with the given name to the uniform map
    */
   GLint addUniform(const std::string &name);

   /**
    * Gets the location of the attribute with the given name from the attribute
    * map
    */
   GLint getAttribute(const std::string &name);

   /**
    * Gets the location of the uniform with the given name from the uniform map
    */
   GLint getUniform(const std::string &name);

   /**
    * Returns whether the program has an attribute with the given name
    */
   bool hasAttribute(const std::string &name);

   /**
    * Returns whether the program has a uniform with the given name
    */
   bool hasUniform(const std::string &name);
};

#endif
