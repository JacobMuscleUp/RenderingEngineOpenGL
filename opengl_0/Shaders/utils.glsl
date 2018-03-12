#ifndef CCKIT_GLSL_UTILS
#define CCKIT_GLSL_UTILS

float Sq(float a) {
    return a * a;
}

vec3 Reflect(vec3 _incident, vec3 _normal) {
    _normal = normalize(_normal);
    return (2 * dot(-_incident, _normal)) * _normal + _incident;
}

#endif //!CCKIT_GLSL_UTILS