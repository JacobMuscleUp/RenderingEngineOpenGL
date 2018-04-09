float InShadow(sampler2D _depthMap, vec3 _fragPosLightSpace)
{
    vec3 fragPosLightSpace = _fragPosLightSpace * 0.5 + 0.5;
	/*if (fragPosLightSpace.x < 0
		|| fragPosLightSpace.y < 0
		|| fragPosLightSpace.z < 0)
		return 0.0;*/
	if (fragPosLightSpace.x > 4
		|| fragPosLightSpace.y > 4
		|| fragPosLightSpace.z > 4)
		return 1.0;
	
    float closestDepth = texture(_depthMap, fragPosLightSpace.xy).r; 
    float currentDepth = fragPosLightSpace.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}