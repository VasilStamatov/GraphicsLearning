#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	float shininess;
};

in vec2 UV;

uniform Material material;

out vec4 color;

void main()
{
	color = texture(material.texture_diffuse1, UV);
}