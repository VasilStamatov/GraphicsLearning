#version 330 core

layout (location = 0) in vec3 vertexPosition_modelSpace;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 normal;

out vec2 UV;
out vec3 surfaceNormal;
out vec3 vertexPosition_worldSpace;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	// projection * view * model (MVP) written reverse for correct output
	mat4 MVP = projMatrix * viewMatrix * modelMatrix;
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelSpace, 1.0f);
	UV = vertexUV;
	
	surfaceNormal = mat3(inverse(transpose(modelMatrix))) * normal;
	
	vertexPosition_worldSpace = vec3(modelMatrix * vec4(vertexPosition_modelSpace, 1.0f));
}