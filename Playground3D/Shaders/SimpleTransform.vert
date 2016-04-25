#version 330 core

layout (location = 0) in vec3 vertexPosition_modelSpace;
layout (location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 MVP;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelSpace,1);
	UV = vertexUV;
}