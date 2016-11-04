#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 weights;

out VS_OUT
{
	vec3 position;
	//vec3 normal;
	vec2 uv;
	mat3 TBN;
} vs_out;

const int MAX_BONES = 100;

uniform mat4 transformMatrix;
uniform mat4 baseModelMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 gBones[MAX_BONES];

void main()
{
	//a vertex can be affected by 4 bones max
	int bone1 = boneIDs.x;
	int bone2 = boneIDs.y;
	int bone3 = boneIDs.z;
	int bone4 = boneIDs.w;

	mat4 BoneTransform = gBones[bone1] * weights.x;
	if (bone2 != -1)
	{
	    BoneTransform += gBones[bone2] * weights.y;
	}
	if (bone3 != -1)
	{
	    BoneTransform += gBones[bone3] * weights.z;
	}
	if (bone4 != -1)
	{
	    BoneTransform += gBones[bone4] * weights.w;
	}
	//the vertex position in bone space
	vec4 bonePos = BoneTransform * vec4(position, 1.0);
	
	//the normals in bone space
	vec4 boneNormal = BoneTransform * vec4(normal, 1.0);
	
	//Get the final model matrix
	mat4 model = baseModelMatrix * transformMatrix;

	//the vertex position in clip space
	gl_Position = (projection * view * model * bonePos);
	
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
		N = normalize(normalMatrix * (-1.0 * boneNormal.xyz));
	}
	else
	{
		N = normalize(normalMatrix * boneNormal.xyz);
	}
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	//The bitangent vector
	vec3 B = cross(T, N);
	
	//Send the TBN matrix in order to revert tangent space normal to world space
	vs_out.TBN = mat3(T, B, N);

	//send the normal of the vertex (unchangeable by transofmations)
	//vs_out.normal = mat3((transpose(inverse(model)))) * boneNormal.xyz;
	//send the worldspace position of the vertex
	vs_out.position = vec3(model * bonePos);
	//send the texture coordinates
	vs_out.uv = uv;
}