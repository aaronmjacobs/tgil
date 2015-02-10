#version 120

uniform mat4 uModelMatrix;

attribute vec3 aPosition;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

void main() {
   vTexCoord = aTexCoord;

   gl_Position = uModelMatrix * vec4(aPosition, 1.0);
}
