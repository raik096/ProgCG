#version 460 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vColor;

uniform vec3 uColor;
uniform sampler2D uTexture;

uniform vec3 uSunDirection;

vec3 lambertDiffuse(vec3 L, vec3 N)
{
    float LN = max(0, dot(L, N));

    vec3 textureColor = texture(uTexture, vTexCoord).rgb;
    
    return uColor * textureColor * LN;
}

void main(void)
{
    vec4 textureColor = texture(uTexture, vTexCoord);
    FragColor = vec4(lambertDiffuse(uSunDirection, normalize(vNormal)), 1);
}
