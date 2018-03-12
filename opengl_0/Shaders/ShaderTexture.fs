#version 330 core
#pragma include "Shaders/utils.glsl"
#define SHADING_TEXTURE
#pragma include "Shaders/lighting.glsl"

#define GAMMA 2.2

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

////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
////////////////////////////////MAIN////////////////////////////////
void main()
{
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    
    vec3 finalColor = vec3(0.0);
    finalColor += CalculateDirectionalLight(material1, fs_in.texCoords, viewDir, fs_in.normal, viewPos, dirLight);
    finalColor += CalculatePointLight(material1, fs_in.texCoords, viewDir, fs_in.normal, fs_in.fragPos, ptLight);
    finalColor += CalculateSpotLight(material1, fs_in.texCoords, viewDir, fs_in.normal, fs_in.fragPos, spotLight);

#ifdef GAMMA
    finalColor = pow(finalColor, vec3(1.0/GAMMA));
#endif

    fragColor = vec4(finalColor, 1.0);
}
