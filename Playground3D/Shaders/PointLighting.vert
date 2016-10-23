#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;

out VS_OUT
{
	vec3 position;
	vec2 uv;
	mat3 TBN;
} vs_out;

uniform mat4 transformMatrix;
uniform mat4 baseModelMatrix;
uniform mat4 projection;
uniform mat4 view;

uniform bool reverseNormals;

void main()
{
	mat4 model = baseModelMatrix * transformMatrix;
	//the vertex position in clip space
	gl_Position = projection * view * model * vec4(position, 1.0);
	
	//get the normal model matrix
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	
	//the tangent vector
	vec3 T = normalize(normalMatrix * tangent);
	//the normal vector
	vec3 N = vec3(0.0, 0.0, 0.0);
	if(reverseNormals)
	{
		//send the normal of the vertex (unchangeable by transofmations)
		//Inversed
		N = normalize(normalMatrix * (-1.0 * normal));
	}
	else
	{
		N = normalize(normalMatrix * normal);
	}
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	//The bitangent vector
	vec3 B = cross(T, N);
	
	//Send the TBN matrix in order to revert tangent space normal to world space
	vs_out.TBN = mat3(T, B, N);
	//send the worldspace position of the vertex
	vs_out.position = vec3(model * vec4(position, 1.0));
	//send the texture coordinates
	vs_out.uv = uv;
}