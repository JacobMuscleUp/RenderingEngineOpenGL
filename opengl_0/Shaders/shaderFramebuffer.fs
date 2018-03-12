#version 330 core

#pragma include "Shaders/postprocess.glsl"

out vec4 fragColor;

in VS_OUT {
	vec2 texCoords;
} fs_in;

uniform sampler2D screenTexture;

const float kernalOffset = 1.0 / 300.0;
const float sharpenKernalWeights[9] 
    = float[] (
        -1, -1, -1
        , -1, 9, -1
        , -1, -1, -1
    );
const float blurKernalWeights[9] 
    = float[] (
        1.0 / 16, 2.0 / 16, 1.0 / 16
        , 2.0 / 16, 4.0 / 16, 2.0 / 16
        , 1.0 / 16, 2.0 / 16, 1.0 / 16
    );

void main()
{
    fragColor = vec4(texture(screenTexture, fs_in.texCoords).rgb, 1.0);// default post-processing
    fragColor = ReinhardToneMapping(fragColor);
    fragColor = PostprocessInversion(fragColor);
    fragColor = PostprocessGrayscale(fragColor);
    fragColor = PostprocessKernal(screenTexture, fs_in.texCoords, kernalOffset, blurKernalWeights);
} 