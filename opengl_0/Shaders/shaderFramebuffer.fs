#version 330 core
out vec4 fragColor;

in vec2 texCoord;

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

vec4 PostprocessInversion(vec4 _fragColor) {
    return vec4(vec3(1.0) - _fragColor.rgb, 1.0);
}

vec4 PostprocessGrayscale(vec4 _fragColor) {
    float average = (_fragColor.r + _fragColor.g + _fragColor.b) * 0.33;
    return vec4(vec3(average), 1.0);
}

vec4 PostprocessKernal(float _offset, float[9] _kernalWeights) {
    vec2 offsets[9] = vec2[] (
        vec2(-_offset, _offset), vec2(0, _offset), vec2(_offset, _offset)
        , vec2(-_offset, 0), vec2(0, 0), vec2(_offset, 0)
        , vec2(-_offset, -_offset), vec2(0, -_offset), vec2(_offset, -_offset)
    );
    vec3 sampleColor[9];
    for (int i = 0; i < 9; i++)
        sampleColor[i] = texture(screenTexture, texCoord + offsets[i]).rgb;
    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++)
        color += sampleColor[i] * _kernalWeights[i];

    return vec4(color, 1.0);
}

void main()
{
    fragColor = vec4(texture(screenTexture, texCoord).rgb, 1.0);// default post-processing
    //fragColor = PostprocessInversion(fragColor);
    //fragColor = PostprocessGrayscale(fragColor);
    fragColor = PostprocessKernal(kernalOffset, blurKernalWeights);
} 