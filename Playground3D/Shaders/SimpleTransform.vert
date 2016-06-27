#version 330 core

layout (location = 0) in vec2 vertexPosition_NDC;
layout (location = 1) in vec2 UV;

out vec2 uv;

void main()
{
	gl_Position = vec4(vertexPosition_NDC.xy, 0.0f, 1.0f);
	uv = UV;
}