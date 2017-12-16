#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 color;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform vec3 monoColor;

void main()
{    
	color = monoColor;
    gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
}