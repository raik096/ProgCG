#version 460 core

out vec4 FragColor;

in vec3 vColor;
in vec2 vTexCoord;

uniform vec3 uColor;
uniform sampler2D texture0;
uniform sampler2D texture1; 

void main(void)
{
    vec4 terrainColor = texture(texture0, vTexCoord);

    vec4 trackColor = texture(texture1, vTexCoord);

    vec4 finalColor = mix(terrainColor, trackColor, 0.5);

    vec4 color = vec4((uColor.x < 0) ? vColor : uColor, 1.0);

    FragColor = finalColor * color;
}
