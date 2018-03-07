#version 330 core

#define GAMMA 2.2

struct Material {
    sampler2D diffuseMap;
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

in VS_OUT {
    vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
} fs_in;

out vec4 fragColor;

uniform Material material1;
uniform DirectionalLight dirLight;
uniform PointLight ptLight;
uniform SpotLight spotLight;
uniform vec3 viewPos; 

float Sq(float a) {
    return a * a;
}

vec3 Reflect(vec3 _incident, vec3 _normal) {
    _normal = normalize(_normal);
    return (2 * dot(-_incident, _normal)) * _normal + _incident;
}

vec3 CalculateDirectionalLight(vec3 _viewDir) {
    // ambient
    vec3 ambient = dirLight.ambient * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // diffuse
    //float diffuseScale = max(0.0, dot(normalize(viewPos - dirLight.dir), fs_in.normal));
    float diffuseScale = max(0.0, dot(normalize(-dirLight.dir), fs_in.normal));
    vec3 diffuse = diffuseScale * dirLight.diffuse * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // specular
    vec3 unitReflected = normalize(Reflect(dirLight.dir, fs_in.normal));
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), material1.shininess);
    vec3 specular = specularScale * dirLight.specular * material1.specular;
    
    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(vec3 _viewDir) {
    vec3 lightDir = ptLight.pos - fs_in.fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
    vec3 ambient = ptLight.ambient * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, fs_in.normal));
    vec3 diffuse = diffuseScale * ptLight.diffuse * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // specular
    vec3 unitReflected = Reflect(-unitLightDir, fs_in.normal);
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), material1.shininess);
    vec3 specular = specularScale * ptLight.specular * material1.specular;
    // attenuation
    //float attenuation = 1.0 / (Sq(lightDir.x) + Sq(lightDir.y) + Sq(lightDir.z));
    float dist = length(lightDir);
    float attenuation = 1.0 / (ptLight.attenConstant + ptLight.attenLinear * dist + ptLight.attenQuadratic * Sq(dist));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(vec3 _viewDir) {
    vec3 lightDir = spotLight.pos - fs_in.fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
    vec3 ambient = spotLight.ambient * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, fs_in.normal));
    vec3 diffuse = diffuseScale * spotLight.diffuse * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // specular
    vec3 unitReflected = Reflect(-unitLightDir, fs_in.normal);
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), material1.shininess);
    vec3 specular = specularScale * spotLight.specular * material1.specular;
    // spot light
    float intensity 
        = (dot(normalize(-spotLight.dir), unitLightDir) - spotLight.outerCutoffCosine)
            / (spotLight.innerCutoffCosine - spotLight.outerCutoffCosine);
    intensity = max(0.0, min(intensity, 1.0));
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;
    // attenuation
    //float attenuation = 1.0 / (Sq(lightDir.x) + Sq(lightDir.y) + Sq(lightDir.z));
    float dist = length(lightDir);
    float attenuation = 1.0 / (ptLight.attenConstant + ptLight.attenLinear * dist + ptLight.attenQuadratic * Sq(dist));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    
    vec3 finalColor = vec3(0.0);
    finalColor += CalculateDirectionalLight(viewDir);
    finalColor += CalculatePointLight(viewDir);
    finalColor += CalculateSpotLight(viewDir);

#ifdef GAMMA
    finalColor = pow(finalColor, vec3(1.0/GAMMA));
#endif

    fragColor = vec4(finalColor, 1.0);
}
