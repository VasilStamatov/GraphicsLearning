#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;

out VS_OUT
{
	vec2 position;
	vec4 color;
	vec2 uv;
} vs_out;

uniform mat4 projection;

void main() 
{
    //transform the world space coordinates from the spritebatch to clip coordinates with the ortho projection matrix
    gl_Position.xy = (projection * vec4(position.xy, 0.0, 1.0)).xy;
    //the z position is zero since we are in 2D
    gl_Position.z = 0.0;
    
    //Indicate that the coordinates are normalized
    gl_Position.w = 1.0;
    
	//pass the world space coordinates
    vs_out.position = position;
    
    vs_out.color = color;
    
	//pass the uv with the v/y inverted
    vs_out.uv = vec2(uv.x, 1.0 - uv.y);
}