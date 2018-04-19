#define SHADOW_ACNE_BIAS 0.0005

float InShadow(sampler2D _depthMap, vec3 _fragPosLightSpaceNDC, vec3 _normal, vec3 _unitLightDir)
{
	if (_fragPosLightSpaceNDC.x < -1.0
		|| _fragPosLightSpaceNDC.y < -1.0
		|| _fragPosLightSpaceNDC.z < -1.0
		|| _fragPosLightSpaceNDC.x > 1.0
		|| _fragPosLightSpaceNDC.y > 1.0
		|| _fragPosLightSpaceNDC.z > 1.0)
		return 0.0;
	if (dot(_normal, _unitLightDir) > 0.0)
		return 0.0;

    vec3 fragPosLightSpace01 = _fragPosLightSpaceNDC * 0.5 + 0.5;
    float closestDepth = texture(_depthMap, fragPosLightSpace01.xy).r; 
    float currentDepth = fragPosLightSpace01.z;
	float bias = max(SHADOW_ACNE_BIAS * 10.0 * (1.0 - dot(_normal, _unitLightDir)), SHADOW_ACNE_BIAS);
    
    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}