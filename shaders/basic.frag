#version 460 core

out vec4 FragColor;

in vec3 vColor;
in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform vec3 uColor;

void main(void)
{
    vec4 textureColor = texture(uTexture, vTexCoord);

    FragColor = textureColor * vec4(uColor, 1);
}
