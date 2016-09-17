#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	sampler2D texture_normal1;
	float shininess;
};

in vec3 out_Position;
in vec3 out_Normal;
in vec2 out_UV;
in vec4 out_weights;

uniform Material material;

out vec4 color;

void main()
{
	float shininess = material.shininess;
	//color = out_weights;
	color = texture(material.texture_diffuse1, out_UV);

}