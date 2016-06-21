#version 330 core
out vec4 color;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};

uniform Material material;

void main()
{
	vec3 texCoords = vec3(texture(material.texture_diffuse1, vec2(1.0f, 1.0f)));
	vec3 texCoords2 = vec3(texture(material.texture_specular1, vec2(1.0f, 1.0f)));

    color = vec4(1.0f); // Set alle 4 vector values to 1.0f
}