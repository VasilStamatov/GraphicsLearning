#version 330 core

uniform sampler2D screenTexture;

in VS_OUT
{
	vec2 uv;
} fs_in;

out vec4 color;

void main()
{ 
    // color = vec4(vec3(1.0 - texture(screenTexture, UV)), 1.0)
	//color = texture(screenTexture, fs_in.uv);
	float depthValue = texture(screenTexture, fs_in.uv).r;
	color = vec4(vec3(depthValue), 1.0);
}