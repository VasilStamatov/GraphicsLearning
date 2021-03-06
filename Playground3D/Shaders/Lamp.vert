#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 transformMatrix;
uniform mat4 baseModelMatrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 model = baseModelMatrix * transformMatrix;
	//the vertex position in clip space
	gl_Position = projection * view * model * vec4(position, 1.0);
}