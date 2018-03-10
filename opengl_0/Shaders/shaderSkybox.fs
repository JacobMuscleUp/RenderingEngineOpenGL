#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 texCoords;
} fs_in;

uniform samplerCube skyboxTexture;

void main()
{
    fragColor = texture(skyboxTexture, fs_in.texCoords);
}