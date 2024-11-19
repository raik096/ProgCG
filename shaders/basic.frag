#version 330
out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;
in vec4 vCoordLS;
in mat3 TBN;

in vec3 vPosVS;
in vec3 vNormalVS;
in vec3 vLDirVS;

uniform vec3 uViewPos;
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbientColor;

uniform vec3 uColor;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform float uGlossiness;


uniform sampler2D uAlbedoTexture;
uniform sampler2D uNormalMap;
uniform sampler2D uShadowMap;

uniform bool uShadowEnable;
uniform float uBias;

vec3 phong ( vec3 L, vec3 V, vec3 N)
{
	float LN = max(0, dot(L, N));
	vec3 R = reflect(-L, N);

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uGlossiness));
	
	float lit = 1.0;

		
	if(uShadowEnable)
	{
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
	}

	float kS = uShininess;
	vec3 color = uColor * texture2D(uAlbedoTexture, vUV.xy).xyz;

	//return vec3(dot(L,N)*N);
	return (uAmbientColor + (LN*uLightColor)*lit + (kS * spec * uLightColor)*lit)*color;
}

void main()
{
	vec3 TextureNormal = texture(uNormalMap, vUV.xy).rgb;
	TextureNormal = normalize(TextureNormal * 2.0 - 1.0);
	TextureNormal = normalize( TBN * TextureNormal);
		
	vec4 color = vec4(phong(-uLightDir, normalize(-vPosVS), TextureNormal),1.0);
	FragColor = color;
};

/*

vec3 phong ( vec3 L, vec3 V, vec3 N){
	//float LN = (dot(L,N) + 0.5f) * 0.5f;
	float LN = max(0, dot(L, N));
	vec3 R = -L+2*dot(L,N)*N;

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uShininess));
	
	float lit = 1.0;
	float bias = clamp(uBias*tan(acos(LN)), uBias ,0.05);

	vec4 pLS = (vCoordLS/vCoordLS.w)*0.5+0.5;
	float depth = texture(uShadowMap,pLS.xy).x;
	//if((depth  + bias) < pLS.z)
		//lit = 0.0;

	float storedDepth;
	for( float  x = 0.0; x < 5.0;x+=1.0)
		for( float y = 0.0; y < 5.0;y+=1.0)
			{
				storedDepth = texture(uShadowMap,pLS.xy+vec2(-1.0+x,-1.0+y)/2048).x;
				if(storedDepth + bias < pLS.z )
					lit  -= 1.0/25.0;
			}

	if(lit == 0)
		return vec3(0, 0, 1);
	else
		return vec3(1, 0, 0);

	if(!uShadowEnable)
	{
		lit = 1.0f;
	}
	vec3 albedo = uColor * texture2D(uAlbedoTexture, vUV.xy).xyz;

	//return (uAmbientColor + (LN*albedo + spec*uSpecularColor)*lit)*uLightColor;

	//-->return (uAmbientColor + (LN*uLightColor)*lit + (0.25 * spec * uLightColor)*lit)*uColor;

	//return  (uAmbientColor* albedo + (LN * albedo) + spec) * uLightColor;
	//return (uAmbientColor*uLightColor + (LN + spec*uLightColor)*lit)*albedo;
}
*/