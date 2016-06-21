#version 330 core

//Interpolated values from vertex shader
in vec2 UV;
in vec3 surfaceNormal;
in vec3 vertexPosition_worldSpace;

out vec4 color;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct	PointLight
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 2

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLights(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir);

void main()
{
	
	//Properties
	vec3 norm = normalize(surfaceNormal);
	vec3 viewDir = normalize(viewPos - vertexPosition_worldSpace);
	
	vec3 result;
	
	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		result += CalcPointLights(pointLights[i], norm, vertexPosition_worldSpace, viewDir);
	}
	
	color = vec4(result, 1.0f);
}

vec3 CalcPointLights(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir)
{
	vec3 lightDir = normalize(_light.position - _fragPos);
	
	//diffuse
	float diff = max(dot(_normal, lightDir), 0.0f);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, _normal);
	float spec = pow(max(dot(_viewDir, reflectDir), 0.0f), material.shininess);
	
	//attenuation
	float rdistance = length(_light.position - _fragPos);
	float attenuation = 1.0f / (_light.constant + _light.linear * rdistance + _light.quadratic * (rdistance * rdistance));
	
	//Combine results
	vec3 ambient  = _light.ambient  * vec3(texture(material.texture_diffuse1, UV));
	vec3 diffuse  = _light.diffuse  * diff * vec3(texture(material.texture_diffuse1, UV));
	vec3 specular = _light.specular * spec * vec3(texture(material.texture_specular1, UV));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + specular + diffuse);
}