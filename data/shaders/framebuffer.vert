#version 330 core

uniform mat4 uModelMatrix;

in vec3 aPosition;
in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
   vTexCoord = aTexCoord;

   gl_Position = uModelMatrix * vec4(aPosition, 1.0);
}
