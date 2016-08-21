#version 330 core
in vec2 UV;
out vec4 color;

uniform sampler2D screenTexture;

void main()
{ 
    color = vec4(vec3(1.0 - texture(screenTexture, UV)), 1.0);
}