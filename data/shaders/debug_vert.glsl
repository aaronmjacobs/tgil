#version 120

uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

attribute vec3 aPosition;
attribute vec3 aColor;

varying vec3 vColor;

void main() {
   vColor = aColor;
   gl_Position = uProjMatrix * uViewMatrix * vec4(aPosition, 1.0);
}
