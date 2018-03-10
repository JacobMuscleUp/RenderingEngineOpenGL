#version 330 core
layout(location = 0) in vec3 aPos;

out VS_OUT {
	vec3 texCoords;
} vs_out;

uniform mat4 projectionMat;
uniform mat4 viewMat;

void main()
{
	vs_out.texCoords = aPos;
	gl_Position = projectionMat * viewMat * vec4(aPos, 1.0);
}