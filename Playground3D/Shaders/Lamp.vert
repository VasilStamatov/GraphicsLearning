#version 330 core

layout (location = 0) in vec3 vertexPosition_modelSpace;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


void main()
{
	// projection * view * model (MVP) written reverse for correct output
	mat4 MVP = projMatrix * viewMatrix * modelMatrix;
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelSpace,1.0f);
}