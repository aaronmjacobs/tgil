#version 120

uniform float uTime;
uniform sampler2D uTexture;
uniform sampler2D uNoiseTexture;
uniform float uFogDensity = 0.45;
uniform vec3 uFogColor = vec3(0.0);

varying vec2 vTexCoord;

void main() {
   vec4 noise = texture2D(uNoiseTexture, vTexCoord);

   vec2 noiseOffset = vec2(1.5, -1.5) * uTime * 0.01;
   vec2 textureOffset = vec2(-0.5, 2.0) * uTime * 0.005;

   vec2 noiseCoord = vTexCoord + noiseOffset + noise.xy * 2.0;
   vec2 textureCoord = vTexCoord + textureOffset;
   textureCoord.x -= noise.y * 0.2;
   textureCoord.y += noise.z * 0.2;

   float p = texture2D(uNoiseTexture, noiseCoord * 2.0).a;

   vec4 color = texture2D(uTexture, textureCoord * 2.0);
   vec4 temp = color * vec4(p * 2.0) + (color * color - 0.1);

   if( temp.r > 1.0 ){ temp.bg += clamp( temp.r - 2.0, 0.0, 100.0 ); }
   if( temp.g > 1.0 ){ temp.rb += temp.g - 1.0; }
   if( temp.b > 1.0 ){ temp.rg += temp.b - 1.0; }

   gl_FragColor = vec4(temp.rgb * 0.5, 1.0);

   /*float depth = gl_FragCoord.z / gl_FragCoord.w;
   depth *= 0.0;
   const float LOG2 = 1.442695;
   float fogFactor = exp2( - uFogDensity * uFogDensity * depth * depth * LOG2 );
   fogFactor = 1.0 - clamp( fogFactor, 0.0, 1.0 );

   gl_FragColor = mix( gl_FragColor, vec4(gl_FragColor.rgb, 0.0), depth );*/
}