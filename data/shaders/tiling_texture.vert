#version 120

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec2 uMeshSize = vec2(100.0);
uniform float uTileSize = 10.0;

attribute vec3 aPosition;

varying vec2 vTexCoord;

void main() {
   // Transforms
   vec4 lPosition = uModelMatrix * vec4(aPosition.xyz, 1.0);
   gl_Position = uProjMatrix * uViewMatrix * lPosition;

   // Texturing
   vTexCoord = ((aPosition.xz + 1.0) / 2.0) * uMeshSize / uTileSize;
}