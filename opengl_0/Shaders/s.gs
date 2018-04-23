/*#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;

void main() 
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}*/

#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

void main()
{
    for(int i = 0, iEnd = gl_in.length(); i < iEnd; ++i) {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}