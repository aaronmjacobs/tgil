#include "FancyAssert.h"
#include "IOUtils.h"
#include "LogHelper.h"
#include "Shader.h"
#include "ShaderAssetManager.h"

#define GLSL(source) "#version 120\n" #source

namespace {

const std::string DEFAULT_VERTEX_SOURCE = GLSL(
   uniform mat4 uModelMatrix;
   uniform mat4 uViewMatrix;
   uniform mat4 uProjMatrix;

   attribute vec3 aPosition;

   void main() {
      gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
   }
);

const std::string DEFAULT_GEOMETRY_SOURCE = GLSL(
   layout(points) in;
   layout(points, max_vertices = 1) out;

   void main() {
      gl_Position = gl_in[0].gl_Position;
      EmitVertex();
      EndPrimitive();
   }
);

const std::string DEFAULT_FRAGMENT_SOURCE = GLSL(
   void main() {
      gl_FragColor = vec4(1.0, 0.35, 0.44, 1.0);
   }
);

SPtr<Shader> DEFAULT_VERTEX_SHADER;
SPtr<Shader> DEFAULT_GEOMETRY_SHADER;
SPtr<Shader> DEFAULT_FRAGMENT_SHADER;

const char* getShaderTypeName(const GLenum type) {
   switch (type) {
      case GL_VERTEX_SHADER:
         return "vertex";
      case GL_GEOMETRY_SHADER:
         return "geometry";
      case GL_FRAGMENT_SHADER:
         return "fragment";
      default:
         return "unknown";
   }
}

std::string getShaderCompileError(SPtr<Shader> shader) {
   GLint infoLogLength;
   glGetShaderiv(shader->getID(), GL_INFO_LOG_LENGTH, &infoLogLength);
   if (infoLogLength < 1) {
      return std::string();
   }

   UPtr<GLchar[]> strInfoLog(new GLchar[infoLogLength]);
   glGetShaderInfoLog(shader->getID(), infoLogLength, NULL, strInfoLog.get());
   if (infoLogLength >= 2 && strInfoLog[infoLogLength - 2] == '\n') {
      strInfoLog[infoLogLength - 2] = '\0'; // If the log ends in a newline, nuke it
   }

   std::string compileError(strInfoLog.get());
   return compileError;
}

SPtr<Shader> getDefaultShader(const GLenum type) {
   const std::string *source;

   switch (type) {
      case GL_VERTEX_SHADER:
         if (DEFAULT_VERTEX_SHADER) {
            return DEFAULT_VERTEX_SHADER;
         } else {
            source = &DEFAULT_VERTEX_SOURCE;
         }
         break;
      case GL_GEOMETRY_SHADER:
         if (DEFAULT_GEOMETRY_SHADER) {
            return DEFAULT_GEOMETRY_SHADER;
         } else {
            source = &DEFAULT_GEOMETRY_SOURCE;
         }
         break;
      case GL_FRAGMENT_SHADER:
         if (DEFAULT_FRAGMENT_SHADER) {
            return DEFAULT_FRAGMENT_SHADER;
         } else {
            source = &DEFAULT_FRAGMENT_SOURCE;
         }
         break;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
         return nullptr;
   }

   SPtr<Shader> shader(std::make_shared<Shader>(type));
   if (!shader->compile(*source)) {
      LOG_MESSAGE("Error compiling default " << getShaderTypeName(type) << " shader: " << getShaderCompileError(shader));
      LOG_FATAL("Unable to compile default " << getShaderTypeName(type) << " shader");
   }

   switch (type) {
      case GL_VERTEX_SHADER:
         DEFAULT_VERTEX_SHADER = shader;
         break;
      case GL_GEOMETRY_SHADER:
         DEFAULT_GEOMETRY_SHADER = shader;
         break;
      case GL_FRAGMENT_SHADER:
         DEFAULT_FRAGMENT_SHADER = shader;
         break;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
         return nullptr;
   }

   return shader;
}

} // namespace

ShaderAssetManager::ShaderAssetManager() {
}

ShaderAssetManager::~ShaderAssetManager() {
}

SPtr<Shader> ShaderAssetManager::loadShader(const std::string &fileName, const GLenum type) {
   ASSERT(type == GL_VERTEX_SHADER || type == GL_GEOMETRY_SHADER || type == GL_FRAGMENT_SHADER, "Invalid shader type: %i", type);

   if (shaderMap.count(fileName) > 0) {
      return shaderMap[fileName];
   }

   folly::Optional<std::string> source = IOUtils::readFromDataFile(fileName);
   if (!source) {
      LOG_WARNING("Unable to load shader from file \"" << fileName << "\", reverting to default shader");
      return getDefaultShader(type);
   }

   SPtr<Shader> shader(std::make_shared<Shader>(type));
   if (!shader->compile(*source)) {
      LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \"" << fileName << "\", reverting to default shader. Error message: \"" << getShaderCompileError(shader) << "\"");
      return getDefaultShader(type);
   }

   shaderMap[fileName] = shader;
   return shader;
}
