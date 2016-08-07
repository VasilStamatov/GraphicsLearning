#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in ivec4 in_boneIDs;
layout (location = 4) in vec4 in_weights;
layout (location = 5) in mat4 in_modelInstanced;

out vec3 Position;
out vec3 Normal;
out vec2 UV;

const int MAX_BONES = 100;

uniform mat4 baseModelMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 gBones[MAX_BONES];

void main()
{
	//a vertex can be affected by 4 bones max
	int bone1 = in_boneIDs.x;
	int bone2 = in_boneIDs.y;
	int bone3 = in_boneIDs.z;
	int bone4 = in_boneIDs.w;

	mat4 BoneTransform = gBones[bone1] * in_weights.x;
	if (bone2 != -1)
	{
	    BoneTransform += gBones[bone2] * in_weights.y;
	}
	if (bone3 != -1)
	{
	    BoneTransform += gBones[bone3] * in_weights.z;
	}
	if (bone4 != -1)
	{
	    BoneTransform += gBones[bone4] * in_weights.w;
	}
	//the vertex position in clip space
	vec4 PosL = BoneTransform * vec4(in_position, 1.0);
	
	mat4 model = baseModelMatrix * in_modelInstanced;
	
	gl_Position = projection * view * model * PosL;
	
	vec4 NormalL = BoneTransform * vec4(in_normal, 0.0);

	//send the normal of the vertex (unchangeable by transofmations)
	Normal = mat3((transpose(inverse(model)))) * NormalL.xyz;
	//send the worldspace position of the vertex
	Position = vec3(model * PosL);
	//send the texture coordinates
	UV = in_uv;
}