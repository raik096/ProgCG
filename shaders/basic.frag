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

uniform float uBias;
uniform sampler2D uSpotShadowMap;
uniform vec2 uSpotShadowMapSize;
uniform mat4 uSpotLightMatrix;

vec3 LambertDiffuse(vec3 L, vec3 N)
{
    float LN = max(0, dot(L, N));
    vec3 textureColor = texture(uTexture, vTexCoord).rgb;
    return uColor * LN;
}

float SpotShadowCalculation(int spotId, vec4 CoordLS)
{
    /*
    if(uSunShadowsEnable == 0)
        return 1;

    float lit = 0.0;
    vec3 projCoords = CoordLS.xyz / CoordLS.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uHeadShadowMap[spotId], projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec2 pixelSize = 1.0/textureSize(uHeadShadowMap[spotId], 0);
    for (float x = -2; x <= 2; x += 1.0)
        for (float y = -2; y <= 2; y += 1.0)
        {
            float storedDepth = texture(uHeadShadowMap[spotId], projCoords.xy + vec2(x, y)*pixelSize).r;
            if (storedDepth + 0.005f < currentDepth)
            lit += 1.0 / 16.0;
        }
    return lit;
    */
    return 1;
}

vec3 CalcSpotLight(vec3 lightPos, vec3 lightDir, vec3 lightColor, vec3 N)
{
    /*
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
    }*/
    return vec3(0.0); // Fuori dal cono, niente luce
}

void main(void)
{
    // Calcolo della luce solare con ombra
    vec3 result = LambertDiffuse(normalize(vec3(1, 1, 1)), normalize(vNormal));

    FragColor = vec4(result, 1.0);
}
