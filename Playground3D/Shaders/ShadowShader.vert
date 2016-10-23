#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 lightSpacePos;
} vs_out;

uniform mat4 baseModelMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;
void main()
{
	//the vertex position in clip space
	gl_Position = projection * view * baseModelMatrix * vec4(position, 1.0);
	//send the normal of the vertex (unchangeable by transofmations)
	vs_out.normal = mat3(transpose(inverse(baseModelMatrix))) * normal;
	//send the worldspace position of the vertex
	vs_out.position = vec3(baseModelMatrix * vec4(position, 1.0));
	//send the texture coordinates
	vs_out.uv = uv;
	//Send the light space position of the vertex (seen through the lights POV)
	vs_out.lightSpacePos = lightSpaceMatrix * vec4(vs_out.position, 1.0);
}