#version 330 core

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec2 uMeshSize = vec2(100.0);
uniform float uTileSize = 10.0;

in vec3 aPosition;
in vec3 aNormal;

out vec2 vTexCoord;

void main() {
   // Transforms
   vec4 lPosition = uModelMatrix * vec4(aPosition.xyz, 1.0);
   gl_Position = uProjMatrix * uViewMatrix * lPosition;

   // Determine which face this is, and therefore which coordinates to use
   vec2 coord;
   if (aNormal.y != 0.0) {
      coord = aPosition.xz;
   } else if (aNormal.x != 0.0) {
      coord = aPosition.zy;
   } else {
      coord = aPosition.xy;
   }

   // Texturing
   vTexCoord = ((coord + 1.0) / 2.0) * uMeshSize / uTileSize;
}