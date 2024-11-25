#version 460 core
#define MAX_LAMPS_AMOUNT 100

out vec4 FragColor;

in vec3 vPos;
in vec3 wPos;
in vec4 wCoordLS;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vColor;
uniform mat4 uModel;

uniform vec3 uColor;
uniform sampler2D uTexture;

uniform vec3 uSunDirection;
uniform sampler2D uShadowMap;
uniform vec2 uShadowMapSize;
uniform float uBias;

uniform int uLampsAmount;
uniform vec3 uLampLights[MAX_LAMPS_AMOUNT];

uniform vec3 uLampLigthColor;
uniform float uLampC1; //Costante
uniform float uLampC2; //Lineare
uniform float uLampC3; //Quadratico


vec3 LambertDiffuse(vec3 L, vec3 N)
{
    float LN = max(0, dot(L, N));

    vec3 textureColor = texture(uTexture, vTexCoord).rgb;
    
    return uColor * textureColor * LN;
}

vec3 CalcPointLigth(vec3 ligthPos, vec3 N)
{
    vec3 ligthDir = normalize(ligthPos - wPos);
    vec3 diffuse = LambertDiffuse(ligthDir, N);

    float distance = length(ligthPos - wPos);
    float attenuation =  min(1 / (uLampC1 + (uLampC2 * distance) + (uLampC3 * distance * distance)), 1);

    vec3 textureColor = texture(uTexture, vTexCoord).rgb;
    return uLampLigthColor * diffuse * attenuation;    
}

float ShadowCalculation(vec4 CoordLS)
{
    float storedDepth;
	float lit = 1.0;

    vec3 projCoords = wCoordLS.xyz / wCoordLS.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    /*
    if(currentDepth - uBias > closestDepth)
        lit = 0.0;
    */

    float bias = clamp(uBias*tan(acos(dot(normalize(vNormal),-uSunDirection))),uBias,0.05); 
    for( float  x = -1.5; x <= 1.5; x+=1.0)
		for( float y = -1.5; y <= 1.5; y+=1.0)
			{
				storedDepth = texture(uShadowMap,projCoords.xy+vec2(x,y)/uShadowMapSize).x;
				if(storedDepth + bias < currentDepth)    
					lit -= 1.0/16.0;
			}


    return lit;
}

void main(void)
{
    //Calcola luce del sole
    vec3 result = LambertDiffuse(uSunDirection, normalize(vNormal)) * ShadowCalculation(wCoordLS);
    float shadow = ShadowCalculation(wCoordLS);

    //Calcola luce dei lampioni
    for(int i = 0; i < uLampsAmount; i++)
    {
        result += CalcPointLigth((vec4(uLampLights[i], 1)).xyz, normalize(vNormal));
        //float distance = length(vec4(uLampLights[i], 1) - vec4(wPos, 1));
        //float attenuation =  1 / (uLampC1 + (uLampC2 * distance) + (uLampC3 * distance * distance));

        //res += attenuation;
    }
    //FragColor = vec4(0, res, 0, 1);
    
    FragColor = vec4(normalize(vNormal), 1);
    FragColor = vec4(result, 1);
    //FragColor = vec4(shadow, shadow, shadow, 1);
    //FragColor = textureColor;
}
