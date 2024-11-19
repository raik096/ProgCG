#version 330
out vec4 FragColor;

in vec3 vNormal;
in vec2 vUV;
in mat3 TBN;

in vec3 vPos;
in vec3 vPosVS;
in vec3 vNormalVS;
in vec3 vLDirVS;
in vec4 vCoordLS;

uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform float uGlossiness;

uniform mat4 uView;
uniform mat4 uModel;

uniform sampler2D uGroundTexture;
uniform sampler2D uRockTexture;
uniform sampler2D uGroundNormalMap;
uniform sampler2D uRockNormalMap;

uniform sampler2D uShadowMap;

uniform bool uShadowEnable;
uniform float uBias;

const float SCALE = 10;

vec3 phong ( vec3 L, vec3 V, vec3 N)
{
	vec3 Normal = normalize(vNormal);
	float steepness = dot(Normal, vec3(0, 1, 0));
	steepness = (steepness - 0.9) / 0.1;
	steepness = max(0, steepness);

	float LN = max(0, dot(L, N));
	vec3 R = reflect(-L, N);

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uGlossiness));
	
	float lit = 1.0;

	vec4 pLS = (vCoordLS/vCoordLS.w)*0.5+0.5;
	float bias = clamp(uBias*tan(acos(LN)), uBias ,0.05);
	//float depth = texture(uShadowMap,pLS.xy).x;

	float storedDepth;
	for( float  x = 0.0; x < 5.0;x+=1.0)
	{
		for( float y = 0.0; y < 5.0;y+=1.0)
		{
			storedDepth = texture(uShadowMap,pLS.xy+vec2(-1.0+x,-1.0+y)/2048).x;
			if(storedDepth + bias < pLS.z )
				lit  -= 1.0/25.0;
		}
	}
	
	vec2 wUV = vec2(vPos.x, vPos.z);
	vec3 ground = texture2D(uGroundTexture, vUV.xy / SCALE).xyz;
	vec3 rock = texture2D(uRockTexture, vUV.xy / SCALE).xyz;
	
	float kS = uShininess;
	vec3 color = mix(rock, ground, steepness);

	//return vec3(dot(L,N)*N);
	return (uAmbientColor + (LN*uLightColor)*lit + (kS * spec * uLightColor)*lit)*color;
}

void main()
{
	vec3 Normal = normalize(vNormal);
	float steepness = dot(Normal, vec3(0, 1, 0));
	steepness = (steepness - 0.9) / 0.1;
	steepness = max(0, steepness);

	vec3 groundN = texture(uGroundNormalMap, vUV.xy / SCALE).rgb;
	groundN = normalize(groundN * 2.0 - 1.0);
	groundN = normalize( TBN * groundN );

	vec3 rockN = texture(uRockNormalMap, vUV.xy / SCALE).rgb;
	rockN = normalize(rockN * 2.0 - 1.0);
	rockN = normalize( TBN * rockN );
	
	vec3 TextureNormal = mix(rockN, groundN, steepness);
		
	vec4 color = vec4(phong(-uLightDir, normalize(-vPosVS), TextureNormal),1.0);
	FragColor = color;
};