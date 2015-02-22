#version 330 core

uniform float uTime;
uniform sampler2D uTexture;
uniform sampler2D uNoiseTexture;
uniform float uFogDensity = 0.45;
uniform vec3 uFogColor = vec3(0.0);

in vec2 vTexCoord;

out vec4 color;

void main() {
   vec4 noise = texture(uNoiseTexture, vTexCoord);

   vec2 noiseOffset = vec2(1.5, -1.5) * uTime * 0.01;
   vec2 textureOffset = vec2(-0.5, 2.0) * uTime * 0.005;

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

   /*float depth = gl_FragCoord.z / gl_FragCoord.w;
   depth *= 0.0;
   const float LOG2 = 1.442695;
   float fogFactor = exp2( - uFogDensity * uFogDensity * depth * depth * LOG2 );
   fogFactor = 1.0 - clamp( fogFactor, 0.0, 1.0 );

   color = mix( color, vec4(color.rgb, 0.0), depth );*/
}