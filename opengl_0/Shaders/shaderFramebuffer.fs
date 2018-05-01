/*#version 330 core

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
    //fragColor = ReinhardToneMapping(fragColor);
    //fragColor = PostprocessInversion(fragColor);
    //fragColor = PostprocessGrayscale(fragColor);
    //fragColor = PostprocessKernal(screenTexture, fs_in.texCoords, kernalOffset, blurKernalWeights);
}*/

#version 330 core

#pragma include "Shaders/postprocess.glsl"

out vec4 fragColor;

in VS_OUT {
	vec2 texCoords;
};

uniform sampler2D screenTexture;
uniform int mode;

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
    fragColor = vec4(texture(screenTexture, texCoords).rgb, 1.0);// default post-processing
    if (mode == 1)
        fragColor = ReinhardToneMapping(fragColor);
    else if (mode == 2)
        fragColor = PostprocessInversion(fragColor);
    else if (mode == 3)
        fragColor = PostprocessGrayscale(fragColor);
    else if (mode == 4)
        fragColor = PostprocessKernal(screenTexture, texCoords, kernalOffset, blurKernalWeights);
}