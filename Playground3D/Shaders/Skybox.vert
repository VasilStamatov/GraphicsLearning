#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 transformMatrix;
uniform mat4 baseModelMatrix;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT
{
	vec3 uv;
} vs_out;

void main()
{
	mat4 model = baseModelMatrix * transformMatrix;
	vec4 pos = projection * view * model * vec4(position, 1.0);
	gl_Position = pos.xyww;
	vs_out.uv = position;
}