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

uniform vec3 uSpotPos;
uniform vec4 uSpotDir;
uniform sampler2D uSpotShadowMap;

vec3 LambertDiffuse(vec3 L, vec3 N)
{
    float LN = max(0, dot(L, N));
    vec3 textureColor = texture(uTexture, vTexCoord).rgb;
    return uColor * LN;
}

float SpotShadowCalculation(vec4 CoordLS)
{
    float lit = 0.0;

    vec3 projCoords = CoordLS.xyz / CoordLS.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    int sampleRadius = 2;
    vec2 pixelSize = 1.0/textureSize(uSpotShadowMap, 0);

    vec3 lightDirection = normalize(uSpotPos - wPos);
    float bias = max(0.00025f * (1.0f - dot(normalize(vNormal), lightDirection)), 0.000005f);

    for(int y = -sampleRadius; y <= sampleRadius; y++)
    {

        for(int x = -sampleRadius; x <= sampleRadius; x++)
        {
            float closestDepth = texture(uSpotShadowMap, projCoords.xy + vec2(x, y) * pixelSize).r;
            if(currentDepth > closestDepth + bias)
                lit += 1.0f;
        }
    }

    lit /= pow((sampleRadius * 2 + 1), 2);
    return  1.0 - lit;
}

vec3 CalcSpotLight(vec3 lightPos, vec3 lightDir, vec3 lightColor)
{
    float outerCutoff = 0.95;
    float cutoff = 0.97;

    float c1 = 1.0;
    float c2 = 0.08;
    float c3 = 0.0044;

    vec3 lightVec = lightPos - wPos;
    vec3 L = normalize(lightVec);

    float theta = dot(L, normalize(-lightDir));
    if (theta > outerCutoff)
    {
        float epsilon = cutoff - outerCutoff;

        float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

        float distance = length(lightVec);
        float attenuation = 1.0 / (c1 + c2 * distance + c3 * (distance * distance));

        return lightColor * intensity * attenuation;
    }
    return vec3(0.0); // Fuori dal cono, niente luce
}

void main(void)
{
    // Calcolo della luce solare con ombra
    vec3 result = LambertDiffuse(normalize(vec3(1, 1, 1)), normalize(vNormal));
    result *= CalcSpotLight(uSpotPos, uSpotDir.xyz, vec3(1, 1, 1));
    result *= SpotShadowCalculation(wCoordLS);

    FragColor = vec4(result, 1.0);
}
