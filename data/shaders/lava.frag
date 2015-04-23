#version 330 core

uniform float uTime;
uniform sampler2D uTexture;
uniform sampler2D uNoiseTexture;

flat in vec2 vTexCoord;

out vec4 color;

void main() {
   vec4 noise = texture(uNoiseTexture, vTexCoord);

   vec2 noiseOffset = vec2(1.5, -1.5) * uTime * 0.01;
   vec2 textureOffset = vec2(-0.5, 2.0) * uTime * 0.001;

   vec2 noiseCoord = vTexCoord + noiseOffset + noise.xy * 2.0;
   vec2 textureCoord = vTexCoord + textureOffset;
   textureCoord.x -= noise.y * 0.2;
   textureCoord.y += noise.z * 0.2;

   float p = texture(uNoiseTexture, noiseCoord * 2.0).a;

   vec4 tColor = texture(uTexture, textureCoord * 2.0);
   vec4 temp = tColor * vec4(p * 2.0) + (tColor * tColor - 0.1);

   if( temp.r > 1.0 ){ temp.bg += clamp( temp.r - 2.0, 0.0, 100.0 ); }
   if( temp.g > 1.0 ){ temp.rb += temp.g - 1.0; }
   if( temp.b > 1.0 ){ temp.rg += temp.b - 1.0; }

   color = vec4(temp.rgb * 0.5, 1.0);
}