#version 460 core

out vec4 FragColor;

in vec3 vPos;
in vec3 wPos;
in vec4 wCoordLS;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vColor;
in vec4 vProjTexCoord[10];

uniform mat4 uModel;
uniform vec3 uColor;
uniform sampler2D uTexture;

uniform vec3 uSunDirection;
uniform sampler2D uShadowMap;
uniform vec2 uShadowMapSize;
uniform float uBias;

#define MAX_LAMPS_AMOUNT 18
uniform int uLampsAmount;
uniform vec3 LampTest;
uniform vec3 uLampLights[MAX_LAMPS_AMOUNT];

uniform vec3 uLampLigthColor;
uniform float uLampC1; // Costante
uniform float uLampC2; // Lineare
uniform float uLampC3; // Quadratico

// texture per projective texture
uniform sampler2D uHeadlightsTexture;
uniform vec3 uProjectorPos[10];
uniform vec3 uProjectorDir[10];

#define MAX_HEADL_AMOUNT 20
uniform int uHeadlightAmount;
uniform vec3 uHeadlights[MAX_HEADL_AMOUNT];

uniform int uHeadlightNAmount;
uniform vec3 uHeadlightN[MAX_HEADL_AMOUNT];

// Direzione e posizione dei fari
uniform vec3 uHeadlightPos;
uniform vec3 uHeadlightDir;
uniform vec3 uHeadlightColor;

// Attenuazione e cutoff
uniform float uHeadlightC1, uHeadlightC2, uHeadlightC3;
uniform float uHeadlightCutOff, uHeadlightOuterCutOff;

vec3 LambertDiffuse(vec3 L, vec3 N)
{
    float LN = max(0, dot(L, N));
    vec3 textureColor = texture(uTexture, vTexCoord).rgb;
    return uColor * textureColor * LN;
}

vec3 CalcPointLight(vec3 lightPos, vec3 N)
{
    vec3 lightDir = normalize(lightPos - wPos);
    vec3 diffuse = LambertDiffuse(lightDir, N);

    float distance = length(lightPos - wPos);
    float attenuation = min(1.0 / (uLampC1 + (uLampC2 * distance) + (uLampC3 * distance * distance)), 1.0);

    return uLampLigthColor * diffuse * attenuation;
}

float ShadowCalculation(vec4 CoordLS)
{
    float lit = 1.0;
    vec3 projCoords = wCoordLS.xyz / wCoordLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Calcolo della bias dell'ombra
    float bias = clamp(uBias * tan(acos(dot(normalize(vNormal), -uSunDirection))), uBias, 0.05);
    for (float x = -1.5; x <= 1.5; x += 1.0)
        for (float y = -1.5; y <= 1.5; y += 1.0)
        {
            float storedDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) / uShadowMapSize).r;
            if (storedDepth + bias < currentDepth)
                lit -= 1.0 / 16.0;
        }
    return lit;
}

vec3 CalcSpotLight(vec3 lightPos, vec3 lightDir, vec3 lightColor, vec3 N)
{
    vec3 lightVec = lightPos - wPos;
    vec3 L = normalize(lightVec);

    float theta = dot(L, normalize(-lightDir));
    if (theta > uHeadlightOuterCutOff)
    {
        float intensity = clamp((theta - uHeadlightOuterCutOff) / (uHeadlightCutOff - uHeadlightOuterCutOff), 0.0, 1.0);

        float distance = length(lightVec);
        float attenuation = 1.0 / (uHeadlightC1 + uHeadlightC2 * distance + uHeadlightC3 * (distance * distance));

        vec3 diffuse = LambertDiffuse(L, N);

        return lightColor * diffuse * attenuation * intensity;
    }
    return vec3(0.0); // Fuori dal cono, niente luce
}

// Funzione per calcolare l'intensità dei fari
float CalculateHeadlightIntensity(vec4 projCoord, vec3 wPos, vec3 lightPos, vec3 lightDir, sampler2D uHeadlightsTexture, float maxDistance) 
{
    projCoord = projCoord / projCoord.w; 
    projCoord = projCoord * 0.5 + 0.5; 

    if (projCoord.x < 0.0 || projCoord.x > 1.0 || projCoord.y < 0.0 || projCoord.y > 1.0) 
        return 0.0;

    vec3 lightVec = wPos - lightPos;
    float distToLight = length(lightVec);
    if (distToLight >= maxDistance || dot(normalize(lightVec), lightDir) <= 0.0) 
        return 0.0;

    vec2 uv = clamp((projCoord).xy, 0.0, 1.0);
    float headlightsIntensity = texture(uHeadlightsTexture, uv).r;

    float dist = distance(uv, vec2(0.5, 0.5)); // Distanza dal centro della texture
    float fadeFactor = smoothstep(0.0, 0.2, dist * 10.0); // Dissolvenza rapida

    return fadeFactor * headlightsIntensity;
}

void main(void)
{
    // Calcolo della luce solare con ombra
    vec3 result = LambertDiffuse(uSunDirection, normalize(vNormal)) * ShadowCalculation(wCoordLS);

    // Calcola luce dei lampioni
    for (int i = 0; i < uLampsAmount; i++)
        result += CalcPointLight(uLampLights[i], normalize(vNormal));

    float maxDistance = 0.1;
    // Calcolo dell'intensità dei fari nel ciclo
    for (int i = 0; i < min(uHeadlightAmount, 10); i++) {
        float intensity = CalculateHeadlightIntensity(vProjTexCoord[i], wPos, uProjectorPos[i], uProjectorDir[i], uHeadlightsTexture, maxDistance);
        result += intensity * 0.1;
    }

    // Calcola luce dei fari (spotlights)
    for (int i = 0; i < uHeadlightAmount; i++)
        result += CalcSpotLight(uHeadlights[i], uHeadlightN[i], uHeadlightColor, normalize(vNormal));

    FragColor = vec4(result, 1.0);
}
