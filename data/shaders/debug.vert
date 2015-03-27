#version 330 core

uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

layout(location = 0) in vec3 aPosition;
layout(location = 3) in vec3 aColor;

out vec3 vColor;

void main() {
   vColor = aColor;
   gl_Position = uProjMatrix * uViewMatrix * vec4(aPosition, 1.0);
}
