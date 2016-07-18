#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 modelInstanced;

out vec3 Position;
out vec3 Normal;
out vec2 UV;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	//the vertex position in clip space
	gl_Position = projection * view * modelInstanced * vec4(position, 1.0);
	//send the normal of the vertex (unchangeable by transofmations)
	Normal = mat3(transpose(inverse(modelInstanced))) * normal;
	//send the worldspace position of the vertex
	Position = vec3(modelInstanced * vec4 (position, 1.0));
	//send the texture coordinates
	UV = uv;
}