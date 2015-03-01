#version 330 core

uniform mat4 uTransform;

in vec3 aPosition;
in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
   vTexCoord = aTexCoord;

   gl_Position = uTransform * vec4(aPosition, 1.0);
}
