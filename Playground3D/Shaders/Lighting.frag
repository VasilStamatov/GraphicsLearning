#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct	DirLight
{
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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

struct	SpotLight
{
	vec3 position;
	vec3 direction;
	
	float cutOff;
	float outerCutOff;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 4

in vec3 surfaceNormal;
in vec3 fragmentPos_worldSpace;
in vec2 UV;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

out vec4 color;

vec3 CalcDirLight(DirLight _light, vec3 _normal, vec3 _viewDir);
vec3 CalcPointLight(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir);
vec3 CalcSpotLight(SpotLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir);

void main()
{
	//Properties
	vec3 norm = normalize(surfaceNormal);
	vec3 viewDir = normalize(viewPos - fragmentPos_worldSpace);
	
	// step 1: calculate directional lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
	// step 2: calculate point lights
	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		result += CalcPointLight(pointLights[i], norm, fragmentPos_worldSpace, viewDir);
	}
	
	// step 3: calculate spot lighting
	
	result += CalcSpotLight(spotLight, norm, fragmentPos_worldSpace, viewDir);
	
	color = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight _light, vec3 _normal, vec3 _viewDir)
{
	vec3 lightDir = normalize(-_light.direction);
	
	//diffuse shading
	float diff = max(dot(_normal, lightDir), 0.0f);
	
	//specular shading
	vec3 reflectDir = reflect(-lightDir, _normal);
	float spec = pow(max(dot(_viewDir, reflectDir), 0.0f), material.shininess);
	
	//Combine result
	vec3 ambient  = _light.ambient  * vec3(texture(material.texture_diffuse1, UV));
	vec3 diffuse  = _light.diffuse  * diff * vec3(texture(material.texture_diffuse1, UV));
	vec3 specular = _light.specular * spec * vec3(texture(material.texture_specular1, UV));
	
	return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir)
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
	
	//Combine result
	vec3 ambient  = _light.ambient  * vec3(texture(material.texture_diffuse1, UV));
	vec3 diffuse  = _light.diffuse  * diff * vec3(texture(material.texture_diffuse1, UV));
	vec3 specular = _light.specular * spec * vec3(texture(material.texture_specular1, UV));
	
	//ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}
vec3 CalcSpotLight(SpotLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir)
{
	vec3 lightDir = normalize(_light.position - _fragPos);
	
	//Calculate ambient lighting
	vec3 ambient = _light.ambient * vec3(texture(material.texture_diffuse1, UV));
	
	//Calculate diffuse lighting
	float diff = max(dot(_normal, lightDir), 0.0f);
	vec3 diffuse = _light.diffuse * (diff * vec3(texture(material.texture_diffuse1, UV)));
	
	//Calculate specular lighting
	vec3 reflectDir = reflect(-lightDir, _normal);
	float spec = pow(max(dot(_viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = _light.specular * (spec * vec3(texture(material.texture_specular1, UV)));
	
	// Attenuation
	float rdistance = length(_light.position - _fragPos);
	float attenuation = 1.0f / (_light.constant + _light.linear * rdistance + _light.quadratic * (rdistance * rdistance));
	
	// SpotLight intensity
	float theta = dot(lightDir, normalize(-_light.direction));
	float epsilon = _light.cutOff - _light.outerCutOff;
	float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0f, 1.0f);
	
	//Apply attenuation and intensity to the shading
	ambient  *= attenuation * intensity; 
	diffuse  *= attenuation * intensity;
	specular *= attenuation * intensity;
	
	//Combine results
	return (diffuse + ambient + specular);
}