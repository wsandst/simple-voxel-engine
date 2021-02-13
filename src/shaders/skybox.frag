#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = mix(texture(skybox, TexCoords), vec4(0.4, 0.7, 0.9, 1.0), 0.5);
}