#version 330 core

out vec4 color;

in VS_OUT
{
	vec2 position;
	vec4 color;
	vec2 uv;
} fs_in;

uniform sampler2D diffuseTexture;

void main() 
{
    vec4 textureColor = texture(diffuseTexture, fs_in.uv);
    color = fs_in.color * textureColor;
}