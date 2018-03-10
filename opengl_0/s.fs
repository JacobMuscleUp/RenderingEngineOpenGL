#version 330 core
out vec4 fragColor;

in VS_OUT {
	vec2 texCoords;
} fs_in;

uniform sampler2D textureMap;

void main()
{    
    fragColor = texture(textureMap, fs_in.texCoords);
}