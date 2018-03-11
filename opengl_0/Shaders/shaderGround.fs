#version 330 core 

#define GAMMA 2.2
#define PARALLAX_MAPPING
//#define STEEP_PARALLAX_MAPPING
#define PARALLAX_OCCLUSION_MAPPING
#define POM_MIN_LAYERS 8
#define POM_MAX_LAYERS 32

struct Material {
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D heightMap;
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
	mat3 matTBN;
} fs_in;

out vec4 fragColor;

uniform Material material1;
uniform DirectionalLight dirLight;
uniform PointLight ptLight;
uniform SpotLight spotLight;
uniform vec3 viewPos; 
uniform mat4 normalModelMat;
uniform float heightScale;

float Sq(float a) {
    return a * a;
}

vec3 Reflect(vec3 _incident, vec3 _normal) {
    _normal = normalize(_normal);
    return (2 * dot(-_incident, _normal)) * _normal + _incident;
}

vec3 CalculateDirectionalLight(vec3 _viewDir, vec3 _normal) {
    // ambient
    vec3 ambient = dirLight.ambient * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // diffuse
    //float diffuseScale = max(0.0, dot(normalize(viewPos - dirLight.dir), _normal));
    float diffuseScale = max(0.0, dot(normalize(-dirLight.dir), _normal));
    vec3 diffuse = diffuseScale * dirLight.diffuse * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // specular
    vec3 unitReflected = normalize(Reflect(dirLight.dir, _normal));
    float specularScale = pow(max(0, dot(unitReflected, _viewDir)), material1.shininess);
    vec3 specular = specularScale * dirLight.specular * material1.specular;
    
    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(vec3 _viewDir, vec3 _normal) {
    vec3 lightDir = ptLight.pos - fs_in.fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
    vec3 ambient = ptLight.ambient * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, _normal));
    vec3 diffuse = diffuseScale * ptLight.diffuse * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // specular
    vec3 unitReflected = Reflect(-unitLightDir, _normal);
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

vec3 CalculateSpotLight(vec3 _viewDir, vec3 _normal) {
    vec3 lightDir = spotLight.pos - fs_in.fragPos
        , unitLightDir = normalize(lightDir);

    // ambient
    vec3 ambient = spotLight.ambient * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // diffuse
    float diffuseScale = max(0.0, dot(unitLightDir, _normal));
    vec3 diffuse = diffuseScale * spotLight.diffuse * texture(material1.diffuseMap, fs_in.texCoords).rgb;
    // specular
    vec3 unitReflected = Reflect(-unitLightDir, _normal);
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

vec2 ParallaxMapping(sampler2D _depthMap, vec2 _texCoords, vec3 _viewDir) { 
    float height = texture(_depthMap, _texCoords).r;     
    return _texCoords - _viewDir.xy / _viewDir.z * (height * heightScale);        
}

vec2 SteepParallaxMapping(sampler2D _depthMap, vec2 _texCoords, vec3 _viewDir, int _minLayers, int _maxLayers) { 
    float numLayers = mix(_maxLayers, _minLayers, abs(dot(vec3(0.0, 0.0, 1.0), _viewDir)));  
    float layerDepth = 1.0 / numLayers;
    
    float currentLayerDepth = 0.0;
    vec2 deltaTexCoords = (_viewDir.xy / _viewDir.z * heightScale) / numLayers;// applied per layer
    
    vec2 currentTexCoords = _texCoords;
    float currentDepthMapValue = texture(_depthMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue) {
        currentDepthMapValue = texture(_depthMap, currentTexCoords -= deltaTexCoords).r;  
        currentLayerDepth += layerDepth;// next layer
    }

    return currentTexCoords;
}

vec2 ParallaxOcclusionMapping(sampler2D _depthMap, vec2 _texCoords, vec3 _viewDir, int _minLayers, int _maxLayers) { 
    float numLayers = mix(_maxLayers, _minLayers, abs(dot(vec3(0.0, 0.0, 1.0), _viewDir)));  
    float layerDepth = 1.0 / numLayers;
    
    float currentLayerDepth = 0.0;
    vec2 deltaTexCoords = (_viewDir.xy / _viewDir.z * heightScale) / numLayers;// applied per layer
    
    vec2 currentTexCoords = _texCoords;
    float currentDepthMapValue = texture(_depthMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue) {
        currentDepthMapValue = texture(_depthMap, currentTexCoords -= deltaTexCoords).r;  
        currentLayerDepth += layerDepth;// next layer
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(_depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    vec2 texCoords = fs_in.texCoords;
#ifdef PARALLAX_MAPPING
#if defined(STEEP_PARALLAX_MAPPING)
    texCoords = SteepParallaxMapping(material1.heightMap, fs_in.texCoords, normalize(transpose(fs_in.matTBN) * viewDir)
        , POM_MIN_LAYERS, POM_MAX_LAYERS); 
#elif defined(PARALLAX_OCCLUSION_MAPPING)
    texCoords = ParallaxOcclusionMapping(material1.heightMap, fs_in.texCoords, normalize(transpose(fs_in.matTBN) * viewDir)
        , POM_MIN_LAYERS, POM_MAX_LAYERS); 
#else
    texCoords = ParallaxMapping(material1.heightMap, fs_in.texCoords, normalize(transpose(fs_in.matTBN) * viewDir)); 
#endif     
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
#endif
    vec3 normal = fs_in.matTBN * (texture(material1.normalMap, texCoords).rgb * 2.0 - 1.0);
    normal = normalize(mat3(normalModelMat) * normal);

    vec3 finalColor = vec3(0.0);
    finalColor += CalculateDirectionalLight(viewDir, normal);
    finalColor += CalculatePointLight(viewDir, normal);
    finalColor += CalculateSpotLight(viewDir, normal);

#ifdef GAMMA
    finalColor = pow(finalColor, vec3(1.0/GAMMA));
#endif

    fragColor = vec4(finalColor, 1.0);
}