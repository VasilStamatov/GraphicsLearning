#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	sampler2D texture_normal1;
	float shininess;
};

// struct	DirLight
// {
	// vec3 direction;
	
	// vec3 ambient;
	// vec3 diffuse;
	// vec3 specular;
// };

// struct PointLight
// {
	// vec3 position;
	
	// vec3 ambient;
	// vec3 diffuse;
	// vec3 specular;
	
	// float constant;
	// float linear;
	// float quadratic;
// };

// struct	SpotLight
// {
	// vec3 position;
	// vec3 direction;
	
	// float cutOff;
	// float outerCutOff;
	
	// vec3 ambient;
	// vec3 diffuse;
	// vec3 specular;
	
	// float constant;
	// float linear;
	// float quadratic;
// };

// #define NR_POINT_LIGHTS 3

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 uv;
} fs_in;

// uniform bool blinn;
// uniform vec3 viewPos;
uniform Material material;
// uniform DirLight dirLight;
// uniform PointLight pointLights[NR_POINT_LIGHTS];
// uniform SpotLight spotLight;

const float PI = 3.14159265;

out vec4 color;

// vec3 CalcDirLight(DirLight _light, vec3 _normal, vec3 _viewDir);
// vec3 CalcPointLight(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir);
// vec3 CalcSpotLight(SpotLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir);

void main()
{
	//Properties
	// vec3 norm = normalize(fs_in.normal);
	// vec3 viewDir = normalize(viewPos - fs_in.position);

	// // step 1: calculate directional lighting
	// vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
	// // step 2: calculate point lights
	// for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	// {
		// result += CalcPointLight(pointLights[i], norm, fs_in.position, viewDir);
	// }
	
	// // step 3: calculate spot lighting	
	// result += CalcSpotLight(spotLight, norm, fs_in.position, viewDir);
	
	vec3 sampleColor = vec3(texture(material.texture_diffuse1, fs_in.uv));
	color = vec4(sampleColor, 1.0f);
}

// vec3 CalcDirLight(DirLight _light, vec3 _normal, vec3 _viewDir)
// {
	// vec3 lightDir = normalize(-_light.direction);
	
	// //diffuse shading
	// float diff = max(dot(_normal, lightDir), 0.0f);
	
	// float spec = 0.0;
	// if(blinn)
	// {
	    // float energyConservation = ( 8.0 + material.shininess ) / ( 8.0 * PI ); 
		// vec3 halfwayDir = normalize(lightDir + _viewDir);
		// spec = energyConservation * pow(max(dot(_normal, halfwayDir), 0.0), material.shininess); 
	// }
	// else
	// {
		// float energyConservation = ( 2.0 + material.shininess ) / ( 2.0 * PI ); 
		// vec3 reflectDir = reflect(-lightDir, _normal);
		// spec = energyConservation * pow(max(dot(_viewDir, reflectDir), 0.0), material.shininess);
	// }
	
	// //Combine result
	// vec3 ambient  = _light.ambient  * vec3(texture(material.texture_diffuse1, fs_in.uv));
	// vec3 diffuse  = _light.diffuse  * diff * vec3(texture(material.texture_diffuse1, fs_in.uv));
	// vec3 specular = _light.specular * spec * vec3(texture(material.texture_specular1, fs_in.uv));
	
	// return (ambient + diffuse + specular);
// }
// vec3 CalcPointLight(PointLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir)
// {
	// vec3 lightDir = normalize(_light.position - _fragPos);
	
	// //diffuse
	// float diff = max(dot(_normal, lightDir), 0.0f);
	
	// float spec = 0.0;
	// if(blinn)
	// {
	    // float energyConservation = ( 8.0 + material.shininess ) / ( 8.0 * PI ); 
		// vec3 halfwayDir = normalize(lightDir + _viewDir);
		// spec = energyConservation * pow(max(dot(_normal, halfwayDir), 0.0), material.shininess); 
	// }
	// else
	// {
		// float energyConservation = ( 2.0 + material.shininess ) / ( 2.0 * PI ); 
		// vec3 reflectDir = reflect(-lightDir, _normal);
		// spec = energyConservation * pow(max(dot(_viewDir, reflectDir), 0.0), material.shininess);
	// }
	
	// //attenuation
	// float rdistance = length(_light.position - _fragPos);
	// float attenuation = 1.0f / (_light.constant + _light.linear * rdistance + _light.quadratic * (rdistance * rdistance));
	
	// //Combine result
	// vec3 ambient  = _light.ambient  * vec3(texture(material.texture_diffuse1, fs_in.uv));
	// vec3 diffuse  = _light.diffuse  * diff * vec3(texture(material.texture_diffuse1, fs_in.uv));
	// vec3 specular = _light.specular * spec * vec3(texture(material.texture_specular1, fs_in.uv));
	
	// ambient *= attenuation;
	// diffuse *= attenuation;
	// specular *= attenuation;
	
	// return (ambient + diffuse + specular);
// }
// vec3 CalcSpotLight(SpotLight _light, vec3 _normal, vec3 _fragPos, vec3 _viewDir)
// {
	// vec3 lightDir = normalize(_light.position - _fragPos);
	
	// //Calculate diffuse shading
	// float diff = max(dot(_normal, lightDir), 0.0f);
	
	// //Calculate specular shadinh
	// float spec = 0.0;
	// if(blinn)
	// {
	    // float energyConservation = ( 8.0 + material.shininess ) / ( 8.0 * PI ); 
		// vec3 halfwayDir = normalize(lightDir + _viewDir);
		// spec = energyConservation * pow(max(dot(_normal, halfwayDir), 0.0), material.shininess); 
	// }
	// else
	// {
		// float energyConservation = ( 2.0 + material.shininess ) / ( 2.0 * PI ); 
		// vec3 reflectDir = reflect(-lightDir, _normal);
		// spec = energyConservation * pow(max(dot(_viewDir, reflectDir), 0.0), material.shininess);
	// }
	
	// // Attenuation
	// float rdistance = length(_light.position - _fragPos);
	// float attenuation = 1.0f / (_light.constant + _light.linear * rdistance + _light.quadratic * (rdistance * rdistance));
	
	// // SpotLight intensity
	// float theta = dot(lightDir, normalize(-_light.direction));
	// float epsilon = _light.cutOff - _light.outerCutOff;
	// float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0f, 1.0f);
	
	// vec3 ambient = _light.ambient * vec3(texture(material.texture_diffuse1, fs_in.uv));
	// vec3 diffuse = _light.diffuse * (diff * vec3(texture(material.texture_diffuse1, fs_in.uv)));
	// vec3 specular = _light.specular * (spec * vec3(texture(material.texture_specular1, fs_in.uv)));

	// //Apply attenuation and intensity to the shading
	// ambient  *= attenuation * intensity; 
	// diffuse  *= attenuation * intensity;
	// specular *= attenuation * intensity;
	
	// //Combine results
	// return (diffuse + ambient + specular);
// }