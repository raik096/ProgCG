#version 330  
out vec4 FragColor;

void main(void) 
{ 
 	FragColor = vec4(gl_FragCoord.z, 0,0.0,1.0);
}
