#version 120

uniform sampler2D uColor;
uniform sampler2D uDepth;

varying vec2 vTexCoord;

void main() {
   gl_FragColor = texture2D(uColor, vTexCoord);
}
