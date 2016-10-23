#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
//layout (location = 3) in mat4 modelInstanced;

out VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 uv;
} vs_out;

uniform mat4 transformMatrix;
uniform mat4 baseModelMatrix;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	mat4 model = baseModelMatrix * transformMatrix;
	//the vertex position in clip space
	gl_Position = projection * view * model * vec4(position, 1.0);
	//send the normal of the vertex (unchangeable by transofmations)
	vs_out.normal = mat3(transpose(inverse(model))) * normal;
	//send the worldspace position of the vertex
	vs_out.position = vec3(model * vec4 (position, 1.0));
	//send the texture coordinates
	vs_out.uv = uv;
}