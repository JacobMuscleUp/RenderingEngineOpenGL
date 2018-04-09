#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 color;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform vec3 monoColor;

void main()
{    
	color = monoColor;
    gl_Position = matProjection * matView * matModel * vec4(aPos, 1.0);
}