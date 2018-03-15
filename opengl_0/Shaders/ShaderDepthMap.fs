#version 330 core

out vec4 fragColor;

in VS_OUT {
	vec2 texCoords;
} fs_in;

uniform sampler2D texture2;

void main()
{
    float depth = texture(texture2, fs_in.texCoords).r;
    fragColor = vec4(vec3(depth), 1.0);
}