#version 330
out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;

uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;

uniform vec3 uColor;

uniform sampler2D uAlbedoTexture;

void main()
{
	vec3 color = uColor * texture2D(uAlbedoTexture, vUV.xy).xyz;
	FragColor = vec4(color, 1.0f);
};