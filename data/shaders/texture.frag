#version 120

uniform sampler2D uTexture;

varying vec2 vTexCoord;

void main() {
   gl_FragColor = texture2D(uTexture, vTexCoord);
}
