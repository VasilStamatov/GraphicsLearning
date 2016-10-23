#version 330 core

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_reflection1;
	sampler2D texture_normal1;
	float shininess;
};

in vec4 worldSpacePos; //position of the fragment in world space (output per emitvertex)

out vec4 color;

uniform vec3 lightPos;
uniform float farPlane;
uniform Material material;

void main()
{ 
	//why am I forced to use shininess ?????
	float annoying = material.shininess;
    // get distance between fragment and light source
	float lightDistance = length(worldSpacePos.xyz - lightPos);
	
	// map to [0;1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;
	
	//write this as modified depth
	gl_FragDepth = lightDistance;
}