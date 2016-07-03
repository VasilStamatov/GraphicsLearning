#version 330 core

out vec4 color;

in vec3 Position;
in vec3 Normal;
in vec2 UV;

void main()
{
    color = vec4(0.04, 0.28, 0.26, 1.0);
}