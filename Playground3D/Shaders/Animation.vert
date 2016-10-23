#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 weights;

out VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 uv;
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

	//send the normal of the vertex (unchangeable by transofmations)
	vs_out.normal = mat3((transpose(inverse(model)))) * boneNormal.xyz;
	//send the worldspace position of the vertex
	vs_out.position = vec3(model * bonePos);
	//send the texture coordinates
	vs_out.uv = uv;
}