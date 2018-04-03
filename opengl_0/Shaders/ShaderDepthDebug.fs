#version 330 core
out vec4 fragColor;

in VS_OUT {
	vec2 texCoords;
} fs_in;

uniform sampler2D depthMap;

void main()
{             
    float depthValue = texture(depthMap, fs_in.texCoords).r;
    fragColor = vec4(vec3(pow(depthValue, 10.0)), 1.0); // orthographic
}