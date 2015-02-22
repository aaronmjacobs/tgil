#version 330 core

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;

in vec3 aPosition;

out vec3 vSkyboxCoord;

mat4 removeTranslation(mat4 matrix) {
   matrix[3][0] = 0.0;
   matrix[3][1] = 0.0;
   matrix[3][2] = 0.0;

   return matrix;
}

void main() {
  vec4 position = uProjMatrix * removeTranslation(uViewMatrix) * vec4(aPosition.xyz, 1.0);
  gl_Position = position.xyww;
  
  vSkyboxCoord = aPosition;
}