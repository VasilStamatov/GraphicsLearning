#version 330 core

//Input: a triangle
layout (triangles) in;
//Output: 6 triangles (6 triangles * 3 vertices each = 18 vertices total)
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6]; //The matrices to transform to light space

out vec4 worldSpacePos; //position of the fragment in world space (output per emitvertex)

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built in variable like gl_Position, which specifies which face to render
		for(int i = 0; i < 3; ++i) //for each triangle's vertices
		{
			worldSpacePos = gl_in[i].gl_Position; //pass the world position of the fragment
			gl_Position = shadowMatrices[face] * worldSpacePos;
			EmitVertex();
		}
		EndPrimitive();
	}
}