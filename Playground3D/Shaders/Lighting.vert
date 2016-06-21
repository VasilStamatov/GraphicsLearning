#version 330 core

layout (location = 0) in vec3 vertexPosition_modelSpace;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 fragmentPos_worldSpace;
out vec3 surfaceNormal;
out vec2 UV;

void main()
{
	// projection * view * model (MVP) written reverse for correct output
	mat4 MVP = projMatrix * viewMatrix * modelMatrix;
	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelSpace,1.0f);
	
	fragmentPos_worldSpace = vec3(modelMatrix * vec4(vertexPosition_modelSpace, 1.0f));
	
	surfaceNormal = mat3(transpose(inverse(modelMatrix))) * normal;
	
	UV = uv;
}