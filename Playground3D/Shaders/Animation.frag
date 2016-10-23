#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	sampler2D texture_normal1;
	float shininess;
};

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 uv;
} fs_in;

uniform vec3 viewPos;
uniform Material material;

const float PI = 3.14159265;

out vec4 color;

void main()
{
	vec3 sampledColor = vec3(texture(material.texture_diffuse1, fs_in.uv));
	color = vec4(sampledColor, 1.0f);
}