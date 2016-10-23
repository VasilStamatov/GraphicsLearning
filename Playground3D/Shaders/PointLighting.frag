#version 330 core

const int POINT_LIGHT_NUMBER = 1;
const float PI = 3.14159265;

out vec4 color;

in VS_OUT
{
	vec3 position;
	vec2 uv;
	mat3 TBN;
} fs_in;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	sampler2D texture_normal1;
	float shininess;
};

struct PointLight
{
	//position of the light
	vec3 position;
	//light color
	vec3 color;
	//intensities
	float ambient;
	float diffuse;
	float specular;
	
	//attenuation
	float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLights[POINT_LIGHT_NUMBER];
uniform Material material;
uniform samplerCube shadowMap;
uniform vec3 cameraPos;
uniform float farPlane;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float CalculateShadow(vec3 _fragPos, vec3 _lightPos);
vec3 CalcPointLight(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir);

void main()
{
	// Obtain normal from normal map in range [0;1]
	vec3 normal = texture(material.texture_normal1, fs_in.uv).rgb;
	//Transform the normal vector to range [-1;1].
	normal = normalize(normal * 2.0 - 1.0); //< the normal in tangent space
	//Use the TBN matrix to transform the tangent space normal to world space
	normal = normalize(fs_in.TBN * normal);
	
	vec3 viewDir = normalize(cameraPos - fs_in.position);
	vec3 result = vec3(0.0, 0.0, 0.0);
	vec3 textureColor = texture(material.texture_diffuse1, fs_in.uv).rgb;

	//Calc all the point lights
	for(int i = 0; i < POINT_LIGHT_NUMBER; ++i)
	{
		result += CalcPointLight(pointLights[i], normal, fs_in.position, viewDir);
	}
	
	result *= textureColor;
	color = vec4(result, 1.0f);
}
float CalculateShadow(vec3 _fragPos, vec3 _lightPos)
{	
	vec3 fragToLight = _fragPos - _lightPos;
	//retrieve the depth value between the current fragment and the light source which we can easily obtain by taking the length of fragToLight
	float currentDepth = length(fragToLight);
	
	// Test for shadows with PCF
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
	
	float viewDistance = length(cameraPos - _fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
	
	float closestDepth = 0.0f;
	
	for(int i = 0; i < samples; ++i)
    {
        closestDepth = texture(shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		//The closestDepth value is currently in the range [0,1] so we first transform it back to [0,far_plane] by multiplying it with far_plane
        closestDepth *= farPlane;
		
        if(currentDepth - bias > closestDepth)
		{
			shadow += 1.0;
		}
    }
    shadow /= float(samples);
	
	// Display closestDepth as debug (to visualize depth cubemap)
    // color = vec4(vec3(closestDepth / farPlane), 1.0);    
	
	return shadow;
}
vec3 CalcPointLight(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir)
{
	vec3 lightDir = normalize(_light.position - _fragPos);
	
	float shadowFactor = CalculateShadow(_fragPos, _light.position);

	//diffuse
	float diff = max(dot(_normal, lightDir), 0.0);
	
	float spec = 0.0;
	
	float energyConservation = ( 8.0 + material.shininess ) / ( 8.0 * PI ); 
	vec3 halfwayDir = normalize(lightDir + _viewDir);
	spec = energyConservation * pow(max(dot(_normal, halfwayDir), 0.0), material.shininess); 
	
	//attenuation
	float rdistance = length(_light.position - _fragPos);
	float attenuation = 1.0f / (_light.constant + _light.linear * rdistance + _light.quadratic * (rdistance * rdistance));
	
	//Combine result
	vec3 ambient  = _light.ambient  * _light.color;
	vec3 diffuse  = _light.diffuse  * diff * _light.color;
	vec3 specular = _light.specular * spec * _light.color;
	
	//ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + (1.0f - shadowFactor) * (diffuse + specular));
}