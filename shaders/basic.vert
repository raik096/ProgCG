#version 330 
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
 
out vec3 vPos;
out vec3 vNormal;
out vec2 vUV;
out vec4 vCoordLS;
out mat3 TBN;

out vec3 vPosVS;
out vec3 vNormalVS;
out vec3 vLDirVS;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uLightMatrix;

uniform mat4 uModel;
uniform vec3 uLightDir;

void main(void) 
{

    vNormal = transpose(inverse(mat3(uModel))) * aNormal.xyz;
    vUV = aUV;

	vPosVS = (uView*uModel*vec4(aPosition, 1.0)).xyz; 
	vLDirVS = (uView*vec4(-uLightDir,0.f)).xyz;
	vNormalVS =  (uView*uModel*vec4(aNormal, 0.0)).xyz;
	
	//vLDirVS = normalize(uView*vec4(1.0, 1.0, 1.0, 0.0f)).xyz;
	//vec3 T = normalize(vec3(uView* uModel * vec4(aTangent, 0.0)));
	//TBNvs = mat3(T, cross(vNormalVS, T), vNormalVS);
	vec3 T = normalize(vec3(uModel * vec4(aTangent, 0.0)));
	TBN = mat3(T, normalize(cross(vNormal, T)), normalize(vNormal));
	
	vCoordLS =  uLightMatrix*uModel*vec4(aPosition, 1.0);
	vPos = (uModel * vec4(aPosition, 1.0)).xyz;
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0); 
}