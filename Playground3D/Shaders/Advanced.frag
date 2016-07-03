#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	float shininess;
};

in vec3 Position;
in vec3 Normal;
in vec2 UV;

uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform Material material;

out vec4 color;

void main()
{
	//Diffuse
	vec4 diffuseColor = texture(material.texture_diffuse1, UV);
	
	//Reflection
	vec3 viewDir = normalize(Position - cameraPos);
	vec3 reflectDir = reflect(viewDir, normalize(Normal));
	float reflectIntensity = texture(material.texture_reflection1, UV).r;
	vec4 reflectColor;
	if (reflectIntensity > 0.1) // Only sample reflections when above a certain treshold
	{
		reflectColor = texture(skybox, reflectDir) * reflectIntensity;
	}
	//combine the results
	color = diffuseColor; //+ reflectColor;
}