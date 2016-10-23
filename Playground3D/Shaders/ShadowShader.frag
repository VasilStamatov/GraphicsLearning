#version 330 core
out vec4 color;

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 lightSpacePos;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform bool shadows;

float CalculateShadow(vec4 _fragPosLightSpace, vec3 _normal, vec3 _ligthDir);

void main()
{
	vec3 textureColor = texture(diffuseTexture, fs_in.uv).rgb;
    vec3 normal = normalize(fs_in.normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = 0.15 * textureColor;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.position);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(cameraPos - fs_in.position);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // Calculate shadow
	float shadow = shadows ? CalculateShadow(fs_in.lightSpacePos, normal, lightDir) : 0.0;                      
    shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * textureColor;    
    
    color = vec4(lighting, 1.0f);
}

float CalculateShadow(vec4 _fragPosLightSpace, vec3 _normal, vec3 _ligthDir)
{
	//perspective divide needed to get the clip space coordinates of the fragment in light space
	//this returns the light space position between [-1 ; 1]
	vec3 projectedCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
	
	//The depth map is in the range of [0;1], so it is needed to transform the NDC coordinates to be between the range [0;1]
	projectedCoords = projectedCoords * 0.5 + 0.5;
	
	//the closes depth value (to the light) this fragment could have
	//float closestDepth = texture(shadowMap, projectedCoords.xy).r;
	
	//the depth value of the current fragment
	float currentDepth = projectedCoords.z;
	
	//calculate the amount of bias based on the surface angle towards the light
	//the result should be between the range [0.5; 0.005]
	float bias = max(0.05 * (1.0 - dot(_normal, _ligthDir)), 0.005);  
	
	float shadow = 0.0;
	
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projectedCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	
	//check if the fragment is outside of the depth map
	if (projectedCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	return shadow;
}