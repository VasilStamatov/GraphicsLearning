#version 330 core

out vec4 color;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	sampler2D texture_normal1;
	float shininess;
};

uniform Material material;

void main()
{
	float shine = material.shininess;
    color = vec4(1.0, 1.0, 1.0, 1.0);
}