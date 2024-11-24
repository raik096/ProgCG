#version 460 core 
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 4) in vec2 aTexCoord;

out vec3 vPos;
out vec3 wPos;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vColor;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;

void main(void) 
{ 
    vPos = (uView*uModel*vec4(aPosition, 1.0)).xyz;
    wPos = (uModel*vec4(aPosition, 1.0)).xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTexCoord = aTexCoord;
	vColor = aColor;
    gl_Position = uProj*uView*uModel*vec4(aPosition, 1.0); 
}
