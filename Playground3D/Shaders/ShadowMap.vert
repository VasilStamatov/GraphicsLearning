#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 transformMatrix;
uniform mat4 baseModelMatrix;

void main()
{
	mat4 model = baseModelMatrix * transformMatrix;
    gl_Position = model * vec4(position, 1.0);
}  