#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 4) in vec2 aTexCoord;

#define amountULP 10

out vec3 vPos;
out vec3 wPos;
out vec4 wCoordLS;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vColor;
out vec4 vProjTexCoord[amountULP];

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uLightMatrix;

uniform mat4 uLPProj;
uniform mat4 uLPView[amountULP];

void main() {
    vPos = (uView * uModel * vec4(aPosition, 1.0)).xyz;
    wPos = (uModel * vec4(aPosition, 1.0)).xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTexCoord = aTexCoord;
    vColor = aColor;
    wCoordLS = uLightMatrix * vec4(wPos, 1);
    
    for (int i = 0; i < amountULP; i++)
        vProjTexCoord[i] =  uLPProj * uLPView[i] * vec4(wPos, 1.0);

    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
}
