#version 330 core
#pragma include "Shaders/utils.glsl"
#define SHADING_DIFFUSE
#pragma include "Shaders/lighting.glsl"
#pragma include "Shaders/shadow.glsl"

#define GAMMA 2.2
#define SHADOW

in VS_OUT {
    vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
    vec3 fragPosLightSpaceNDC;
} fs_in;

out vec4 fragColor;

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight ptLight;
uniform SpotLight spotLight;
uniform vec3 viewPos; 

#ifdef SHADOW
uniform sampler2D depthMap;
#endif

////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    
    vec3 finalColor = vec3(0.0);
    finalColor += CalculateDirectionalLight(material, viewDir, fs_in.normal, viewPos, dirLight);
    finalColor += CalculatePointLight(material, viewDir, fs_in.normal, fs_in.fragPos, ptLight);
    finalColor += CalculateSpotLight(material, viewDir, fs_in.normal, fs_in.fragPos, spotLight);

#ifdef GAMMA
    finalColor = pow(finalColor, vec3(1.0/GAMMA));
#endif

#ifdef SHADOW
    finalColor = (1.0 - 0.7 * InShadow(depthMap, fs_in.fragPosLightSpaceNDC, fs_in.normal, normalize(dirLight.dir))) * finalColor;
#endif
    fragColor = vec4(finalColor, 1.0);
}
/////////////material1.diffuse