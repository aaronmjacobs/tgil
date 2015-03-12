#version 330 core

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec2 uUvScale = vec2(3.0);

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
   vTexCoord = uUvScale * aTexCoord;

   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
}