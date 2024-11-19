#version 330
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;

void main(void) 
{
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
}
