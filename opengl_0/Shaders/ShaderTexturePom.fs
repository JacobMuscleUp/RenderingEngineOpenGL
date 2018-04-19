#version 330 core 
#pragma include"Shaders/effects.glsl"   
//#pragma include "Shaders/utils.glsl"
#define SHADING_TEXTURE_POM
#pragma include "Shaders/lighting.glsl"
#pragma include "Shaders/shadow.glsl"

#define GAMMA 2.2
#define PARALLAX_MAPPING
//#define STEEP_PARALLAX_MAPPING
#define PARALLAX_OCCLUSION_MAPPING
#define POM_MIN_LAYERS 8
#define POM_MAX_LAYERS 32
#define SHADOW

in VS_OUT {
    vec2 texCoords;
	vec3 fragPos;
    vec3 normal;
	mat3 matTBN;
    vec3 fragPosLightSpaceNDC;
} fs_in;

out vec4 fragColor;

uniform Material material1;
uniform DirectionalLight dirLight;
uniform PointLight ptLight;
uniform SpotLight spotLight;
uniform vec3 viewPos; 
uniform mat4 matNormalModel;
uniform float heightScale;

#ifdef SHADOW
uniform sampler2D depthMap;
#endif



////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    vec2 texCoords = fs_in.texCoords;
#ifdef PARALLAX_MAPPING
#if defined(STEEP_PARALLAX_MAPPING)
    texCoords = SteepParallaxMapping(material1.heightMap, fs_in.texCoords, normalize(transpose(fs_in.matTBN) * viewDir), heightScale
        , POM_MIN_LAYERS, POM_MAX_LAYERS); 
#elif defined(PARALLAX_OCCLUSION_MAPPING)
    texCoords = ParallaxOcclusionMapping(material1.heightMap, fs_in.texCoords, normalize(transpose(fs_in.matTBN) * viewDir), heightScale
        , POM_MIN_LAYERS, POM_MAX_LAYERS); 
#else
    texCoords = ParallaxMapping(material1.heightMap, fs_in.texCoords, normalize(transpose(fs_in.matTBN) * viewDir), heightScale); 
#endif     
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
#endif
    vec3 normal = fs_in.matTBN * (texture(material1.normalMap, texCoords).rgb * 2.0 - 1.0);
    normal = normalize(mat3(matNormalModel) * normal);

    vec3 finalColor = vec3(0.0);
    finalColor += CalculateDirectionalLight(material1, fs_in.texCoords, viewDir, normal, viewPos, dirLight);
    finalColor += CalculatePointLight(material1, fs_in.texCoords, viewDir, normal, fs_in.fragPos, ptLight);
    finalColor += CalculateSpotLight(material1, fs_in.texCoords, viewDir, normal, fs_in.fragPos, spotLight);

#ifdef GAMMA
    finalColor = pow(finalColor, vec3(1.0/GAMMA));
#endif

#ifdef SHADOW
    finalColor = (1.0 - 0.7 * InShadow(depthMap, fs_in.fragPosLightSpaceNDC, normal, normalize(dirLight.dir))) * finalColor;
#endif

    fragColor = vec4(finalColor, 1.0);
}