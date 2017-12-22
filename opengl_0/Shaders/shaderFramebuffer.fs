#version 330 core
out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D screenTexture;

void main()
{
    fragColor = vec4(texture(screenTexture, texCoord).rgb, 1.0);// default post-processing
    fragColor = vec4(vec3(1.0) - texture(screenTexture, texCoord).rgb, 1.0);
} 