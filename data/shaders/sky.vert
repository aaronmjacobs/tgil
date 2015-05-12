#version 330 core

layout(location = 0) in vec3 aPosition;

void main() {
   // Draw at max depth
   gl_Position = vec4(aPosition.xy, 1.0, 1.0);
}