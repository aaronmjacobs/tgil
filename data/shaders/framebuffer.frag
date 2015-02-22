#version 120

uniform sampler2D uColor;
uniform sampler2D uDepth;
uniform float uBrightness = 1.0;

varying vec2 vTexCoord;

void main() {
   gl_FragColor = texture2D(uColor, vTexCoord) * vec4(uBrightness);
}
