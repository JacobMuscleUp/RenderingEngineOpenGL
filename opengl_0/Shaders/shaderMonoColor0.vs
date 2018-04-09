#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{    
	color = aColor;
    gl_Position = matProjection * matView * matModel * vec4(aPos, 1.0);
}