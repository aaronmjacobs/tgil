#include "FancyAssert.h"
#include "IOUtils.h"
#include "LogHelper.h"
#include "Shader.h"
#include "ShaderAssetManager.h"
#include "ShaderProgram.h"

#define GLSL(source) "#version 330 core\n" #source

namespace {

const std::string VERTEX_EXTENSION = ".vert";
const std::string GEOMETRY_EXTENSION = ".geom";
const std::string FRAGMENT_EXTENSION = ".frag";

const std::string DEFAULT_VERTEX_SOURCE = GLSL(
   uniform mat4 uModelMatrix;
   uniform mat4 uViewMatrix;
   uniform mat4 uProjMatrix;

   in vec3 aPosition;

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
   out vec4 color;

   void main() {
      color = vec4(1.0, 0.35, 0.44, 1.0);
   }
);

SPtr<Shader> DEFAULT_VERTEX_SHADER;
SPtr<Shader> DEFAULT_GEOMETRY_SHADER;
SPtr<Shader> DEFAULT_FRAGMENT_SHADER;
SPtr<ShaderProgram> DEFAULT_SHADER_PROGRAM;

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

std::string getShaderError(GLuint id, bool isProgram) {
   GLint infoLogLength;

   if (isProgram) {
      glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
   } else {
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
   }

   if (infoLogLength < 1) {
      return std::string();
   }

   UPtr<GLchar[]> strInfoLog(new GLchar[infoLogLength]);

   if (isProgram) {
      glGetProgramInfoLog(id, infoLogLength, NULL, strInfoLog.get());
   } else {
      glGetShaderInfoLog(id, infoLogLength, NULL, strInfoLog.get());
   }

   // If the log ends in a newline, nuke it
   if (infoLogLength >= 2 && strInfoLog[infoLogLength - 2] == '\n') {
      strInfoLog[infoLogLength - 2] = '\0';
   }

   return std::string(strInfoLog.get());
}

std::string getShaderCompileError(SPtr<Shader> shader) {
   return getShaderError(shader->getID(), false);
}

std::string getShaderLinkError(SPtr<ShaderProgram> shaderProgram) {
   return getShaderError(shaderProgram->getID(), true);
}

const std::string& getDefaultShaderSource(const GLenum type) {
   switch (type) {
      case GL_VERTEX_SHADER:
         return DEFAULT_VERTEX_SOURCE;
      case GL_GEOMETRY_SHADER:
         return DEFAULT_GEOMETRY_SOURCE;
      case GL_FRAGMENT_SHADER:
         return DEFAULT_FRAGMENT_SOURCE;
      default:
         ASSERT(false, "Invalid shader type: %i", type);
         return DEFAULT_VERTEX_SOURCE;
   }
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
      LOG_MESSAGE("Error compiling default " << getShaderTypeName(type) << " shader. Error message: \"" << getShaderCompileError(shader) << "\"");
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

SPtr<ShaderProgram> getDefaultShaderProgram() {
   if (DEFAULT_SHADER_PROGRAM) {
      return DEFAULT_SHADER_PROGRAM;
   }

   DEFAULT_SHADER_PROGRAM = std::make_shared<ShaderProgram>();
   DEFAULT_SHADER_PROGRAM->attach(getDefaultShader(GL_VERTEX_SHADER));
   DEFAULT_SHADER_PROGRAM->attach(getDefaultShader(GL_FRAGMENT_SHADER));

   if (!DEFAULT_SHADER_PROGRAM->link()) {
      LOG_MESSAGE("Error linking default shader program. Error message: \"" << getShaderLinkError(DEFAULT_SHADER_PROGRAM) << "\"");
      LOG_FATAL("Unable to link default shader");
   }

   return DEFAULT_SHADER_PROGRAM;
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
      shader = getDefaultShader(type);
   }

   shaderMap[fileName] = shader;
   return shader;
}

// TODO Handle adding of uniforms / attributes
SPtr<ShaderProgram> ShaderAssetManager::loadShaderProgram(const std::string &fileName) {
   if (shaderProgramMap.count(fileName) > 0) {
      return shaderProgramMap[fileName];
   }

   SPtr<ShaderProgram> shaderProgram = std::make_shared<ShaderProgram>();
   std::string vertexFileName = fileName + VERTEX_EXTENSION;
   std::string geometryFileName = fileName + GEOMETRY_EXTENSION;
   std::string fragmentFileName = fileName + FRAGMENT_EXTENSION;

   if (IOUtils::canReadData(vertexFileName)) {
      shaderProgram->attach(loadShader(vertexFileName, GL_VERTEX_SHADER));
   }

   if (IOUtils::canReadData(geometryFileName)) {
      shaderProgram->attach(loadShader(geometryFileName, GL_GEOMETRY_SHADER));
   }

   if (IOUtils::canReadData(fragmentFileName)) {
      shaderProgram->attach(loadShader(fragmentFileName, GL_FRAGMENT_SHADER));
   }

   if (!shaderProgram->link()) {
      LOG_WARNING("Unable to link '" << fileName << "' shader program, reverting to default shader program. Error message: \"" << getShaderLinkError(shaderProgram) << "\"");
      shaderProgram = getDefaultShaderProgram();
   }

   shaderProgramMap[fileName] = shaderProgram;
   return shaderProgram;
}

void ShaderAssetManager::reloadShaders() {
   // TODO Only reload if files have been updated (check file modification time)

   for (ShaderMap::iterator itr = shaderMap.begin(); itr != shaderMap.end(); ++itr) {
      const std::string& fileName = itr->first;
      SPtr<Shader> shader = itr->second;

      folly::Optional<std::string> source = IOUtils::readFromDataFile(fileName);
      if (!source) {
         LOG_WARNING("Unable to load shader from file \"" << fileName << "\", not reloading");
         continue;
      }

      if (!shader->compile(*source)) {
         LOG_WARNING("Unable to compile " << getShaderTypeName(shader->getType()) << " shader loaded from file \"" << fileName << "\", reverting to default shader. Error message: \"" << getShaderCompileError(shader) << "\"");

         const std::string &defaultSource = getDefaultShaderSource(shader->getType());
         if (!shader->compile(defaultSource)) {
            LOG_MESSAGE("Error compiling default " << getShaderTypeName(shader->getType()) << " shader. Error message: \"" << getShaderCompileError(shader) << "\"");
            LOG_FATAL("Unable to compile default " << getShaderTypeName(shader->getType()) << " shader");
         }
      }
   }

   for (ShaderProgramMap::iterator itr = shaderProgramMap.begin(); itr != shaderProgramMap.end(); ++itr) {
      const std::string& fileName = itr->first;
      SPtr<ShaderProgram> shaderProgram = itr->second;

      if (!shaderProgram->link()) {
         LOG_WARNING("Unable to link '" << fileName << "' shader program. Error message: \"" << getShaderLinkError(shaderProgram) << "\"");
      }

      // TODO Clear and re-add all uniforms / attributes
   }
}
