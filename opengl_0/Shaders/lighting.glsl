#ifndef CCKIT_GLSL_LIGHTING
#define CCKIT_GLSL_LIGHTING

#pragma include "Shaders/utils.glsl"

struct Material {
#ifdef SHADING_DIFFUSE
	vec3 diffuse;
#else
    sampler2D diffuseMap;
#endif
#ifdef SHADING_TEXTURE_POM
	sampler2D normalMap;
    sampler2D heightMap;
#endif
    vec3 specular;
    int shininess;// recommended value = 32
};
struct DirectionalLight {
    vec3 dir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float attenConstant;
    float attenLinear;
    float attenQuadratic;
};
struct SpotLight {
    vec3 pos;
    vec3 dir;
    float innerCutoffCosine;
    float outerCutoffCosine;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float attenConstant;
    float attenLinear;
    float attenQuadratic;
};

vec3 CalculateDirectionalLight(Material _material
#ifndef SHADING_DIFFUSE
	, vec2 _texCoords
#endif
	, vec3 _viewDir, vec3 _normal, vec3 _viewPos, DirectionalLight _dirLight) {
    // ambient
#ifdef SHADING_DIFFUSE
	vec3 ambient = _dirLight.ambient * _material.diffuse; 
#else
    vec3 ambient = _dirLight.ambient * texture(_material.diffuseMap, _texCoords).rgb;
#endif
    // diffuse
    //float diffuseScale = max(0.0, dot(normalize(_viewPos - _dirLight.dir), _normal));
    float diffuseScale = max(0.0, dot(normalize(-_dirLight.dir), _normal));
#ifdef SHADING_DIFFUSE
	vec3 diffuse = diffuseScale * _dirLight.diffuse * _material.diffuse;
#else
    vec3 diffuse = diffuseScale * _dirLight.diffuse * texture(_material.diffuseMap, _texCoords).rgb;
#endif
    // specular
    vec3 unitReflected = normalize(Reflect(_dirLight.dir, _normal));
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), _material.shininess);
    vec3 specular = specularScale * _dirLight.specular * _material.specular;
    
    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(Material _material
#ifndef SHADING_DIFFUSE
	, vec2 _texCoords
#endif
	, vec3 _viewDir, vec3 _normal, vec3 _fragPos, PointLight _ptLight) {
    vec3 lightDir = _ptLight.pos - _fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
#ifdef SHADING_DIFFUSE
	vec3 ambient = _ptLight.ambient * _material.diffuse;
#else
    vec3 ambient = _ptLight.ambient * texture(_material.diffuseMap, _texCoords).rgb;
#endif
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, _normal));
#ifdef SHADING_DIFFUSE
	vec3 diffuse = diffuseScale * _ptLight.diffuse * _material.diffuse;
#else
    vec3 diffuse = diffuseScale * _ptLight.diffuse * texture(_material.diffuseMap, _texCoords).rgb;
#endif
    // specular
    vec3 unitReflected = Reflect(-unitLightDir, _normal);
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), _material.shininess);
    vec3 specular = specularScale * _ptLight.specular * _material.specular;
    // attenuation
    //float attenuation = 1.0 / (Sq(lightDir.x) + Sq(lightDir.y) + Sq(lightDir.z));
    float dist = length(lightDir);
    float attenuation = 1.0 / (_ptLight.attenConstant + _ptLight.attenLinear * dist + _ptLight.attenQuadratic * Sq(dist));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(Material _material
#ifndef SHADING_DIFFUSE
	, vec2 _texCoords
#endif
	, vec3 _viewDir, vec3 _normal, vec3 _fragPos, SpotLight _spotLight) {
    vec3 lightDir = _spotLight.pos - _fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
#ifdef SHADING_DIFFUSE
	vec3 ambient = _spotLight.ambient * _material.diffuse;
#else
    vec3 ambient = _spotLight.ambient * texture(_material.diffuseMap, _texCoords).rgb;
#endif
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, _normal));
#ifdef SHADING_DIFFUSE
	vec3 diffuse = diffuseScale * _spotLight.diffuse * _material.diffuse;
#else
    vec3 diffuse = diffuseScale * _spotLight.diffuse * texture(_material.diffuseMap, _texCoords).rgb;
#endif
    // specular
    vec3 unitReflected = Reflect(-unitLightDir, _normal);
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), _material.shininess);
    vec3 specular = specularScale * _spotLight.specular * _material.specular;
    // spot light
    float intensity 
        = (dot(normalize(-_spotLight.dir), unitLightDir) - _spotLight.outerCutoffCosine)
            / (_spotLight.innerCutoffCosine - _spotLight.outerCutoffCosine);
    intensity = max(0.0, min(intensity, 1.0));
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;
    // attenuation
    //float attenuation = 1.0 / (Sq(lightDir.x) + Sq(lightDir.y) + Sq(lightDir.z));
    float dist = length(lightDir);
    float attenuation = 1.0 / (_spotLight.attenConstant + _spotLight.attenLinear * dist + _spotLight.attenQuadratic * Sq(dist));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

#endif //!CCKIT_GLSL_LIGHTING