#ifndef CCKIT_GLSL_POSTPROCESS
#define CCKIT_GLSL_POSTPROCESS

vec4 PostprocessInversion(vec4 _fragColor) {
    return vec4(vec3(1.0) - _fragColor.rgb, 1.0);
}

vec4 PostprocessGrayscale(vec4 _fragColor) {
    float average = (_fragColor.r + _fragColor.g + _fragColor.b) * 0.33;
    return vec4(vec3(average), 1.0);
}

vec4 PostprocessKernal(sampler2D _texture, vec2 _texCoords, float _offset, float[9] _kernalWeights) {
    vec2 offsets[9] = vec2[] (
        vec2(-_offset, _offset), vec2(0, _offset), vec2(_offset, _offset)
        , vec2(-_offset, 0), vec2(0, 0), vec2(_offset, 0)
        , vec2(-_offset, -_offset), vec2(0, -_offset), vec2(_offset, -_offset)
    );
    vec3 sampleColor[9];
    for (int i = 0; i < 9; i++)
        sampleColor[i] = texture(_texture, _texCoords + offsets[i]).rgb;
    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++)
        color += sampleColor[i] * _kernalWeights[i];

    return vec4(color, 1.0);
}

vec4 ReinhardToneMapping(vec4 _color) {
    vec3 color = _color.xyz;
    return vec4(color / (color + vec3(1.0)), 1.0);
}

#endif //!CCKIT_GLSL_POSTPROCESS