#version 330 core

uniform mat4 uTransform;

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
   vTexCoord = aTexCoord;

   gl_Position = uTransform * vec4(aPosition, 1.0);
}
