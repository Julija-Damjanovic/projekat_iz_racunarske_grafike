#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube pozadina;

void main()
{    
    FragColor = texture(pozadina, TexCoords);
}