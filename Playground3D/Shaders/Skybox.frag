#version 330 core

in VS_OUT
{
	vec3 uv;
} fs_in; 

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
uniform samplerCube skybox;

void main()
{
	float shine = material.shininess;
	color = texture(skybox, fs_in.uv);
}