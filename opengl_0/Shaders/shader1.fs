#version 330 core

struct Material {
    vec3 diffuse;
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

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

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
    vec3 ambient = dirLight.ambient * material1.diffuse;
    // diffuse
    float diffuseScale = max(0.0, dot(-dirLight.dir, normal));
    vec3 diffuse = diffuseScale * dirLight.diffuse * material1.diffuse;
    // specular
    vec3 unitReflected = normalize(Reflect(dirLight.dir, normal));
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), material1.shininess);
    vec3 specular = specularScale * dirLight.specular * material1.specular;

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(vec3 _viewDir) {
    vec3 lightDir = ptLight.pos - fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
    vec3 ambient = ptLight.ambient * material1.diffuse;
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, normal));
    vec3 diffuse = diffuseScale * ptLight.diffuse * material1.diffuse;
    // specular
    vec3 unitReflected = reflect(-unitLightDir, normal);
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), material1.shininess);
    vec3 specular = specularScale * ptLight.specular * material1.specular;
    // attenuation
    float attenuation = 1.0 / (Sq(lightDir.x) + Sq(lightDir.y) + Sq(lightDir.z));
    //float dist = length(lightDir);
    //float attenuation = 1.0 / (ptLight.attenConstant + ptLight.attenLinear * dist + ptLight.attenQuadratic * Sq(dist));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(vec3 _viewDir) {
    vec3 lightDir = spotLight.pos - fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
    vec3 ambient = ptLight.ambient * material1.diffuse;
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, normal));
    vec3 diffuse = diffuseScale * ptLight.diffuse * material1.diffuse;
    // specular
    vec3 unitReflected = reflect(-unitLightDir, normal);
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), material1.shininess);
    vec3 specular = specularScale * ptLight.specular * material1.specular;
    // spot light
    float intensity 
        = (dot(normalize(-spotLight.dir), unitLightDir) - spotLight.outerCutoffCosine)
            / (spotLight.innerCutoffCosine - spotLight.outerCutoffCosine);
    intensity = max(0.0, min(intensity, 1.0));
    //ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;
    // attenuation
    float attenuation = 1.0 / (Sq(lightDir.x) + Sq(lightDir.y) + Sq(lightDir.z));
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
    vec3 viewDir = normalize(viewPos - fragPos);
    
    vec3 finalColor = vec3(0.0);
    finalColor += CalculateDirectionalLight(viewDir);
    finalColor += CalculatePointLight(viewDir);
    finalColor += CalculateSpotLight(viewDir);

    fragColor = vec4(finalColor, 1.0);
}
/////////////material1.diffuse