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

#define MAX_HEADL_AMOUNT 100
uniform int uHeadlightAmount;
uniform vec3 uHeadlights[MAX_HEADL_AMOUNT];

// Direzione e posizione dei fari
uniform vec3 uHeadlightPos;
uniform vec3 uHeadlightDir;
uniform vec3 uHeadlightColor;

// Attenuazione e cutoff
uniform float uHeadlightC1;
uniform float uHeadlightC2;
uniform float uHeadlightC3;
uniform float uHeadlightCutOff;
uniform float uHeadlightOuterCutOff;


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

vec3 CalcSpotLight(vec3 lightPos, vec3 lightDir, vec3 lightColor, vec3 N) {
    vec3 lightVec = lightPos - wPos; 
    vec3 L = normalize(lightVec);

    // Controllo angolo del cono
    float theta = dot(L, normalize(-lightDir));
    if (theta > uHeadlightOuterCutOff) {
        // Calcolo intensità in base all'angolo
        float intensity = clamp((theta - uHeadlightOuterCutOff) / 
                                 (uHeadlightCutOff - uHeadlightOuterCutOff), 0.0, 1.0);

        // Calcola attenuazione
        float distance = length(lightVec);
        float attenuation = 1.0 / (uHeadlightC1 + uHeadlightC2 * distance + 
                                   uHeadlightC3 * (distance * distance));

        // Diffuse lighting
        vec3 diffuse = LambertDiffuse(L, N);
        
        // Combinazione colore/intensità
        return lightColor * diffuse * attenuation * intensity;
    }
    return vec3(0.0); // Fuori dal cono, niente luce
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

    // Calcola luce dei lampioni
    for (int i = 0; i < uLampsAmount; i++) {
        result += CalcPointLigth(uLampLights[i], normalize(vNormal));
    }

    // Calcola luce dei fari
    for (int i = 0; i < uHeadlightAmount; i++) {
        result += CalcSpotLight(uHeadlights[i], uHeadlightDir, uHeadlightColor, normalize(vNormal));
    }
    // Colore finale
    FragColor = vec4(result, 1.0);
}
