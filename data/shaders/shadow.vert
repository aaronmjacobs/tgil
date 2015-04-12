#version 330 core

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uLightDir;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

void main() {
   const float baseOffset = 0.05;
   const float maxOffset = 0.1;

   float cos = dot(normalize(aNormal), normalize(-uLightDir));
   float offsetAmount = baseOffset * abs(tan(acos(cos)));
   offsetAmount = clamp(offsetAmount, 0.0, maxOffset);
   vec3 offset = vec3(normalize(aNormal) * offsetAmount);

   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition - offset, 1.0);
}
