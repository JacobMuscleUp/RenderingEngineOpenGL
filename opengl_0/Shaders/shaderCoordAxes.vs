#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

void main()
{    
	color = aColor;
    gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
}