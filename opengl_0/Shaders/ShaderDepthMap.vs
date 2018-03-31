#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 matDepthMapSpace;
uniform mat4 matModel;

void main()
{
    gl_Position = matDepthMapSpace * matModel * vec4(aPos, 1.0); 
}  