#ifndef CCKIT_GLSL_EFFECTS
#define CCKIT_GLSL_EFFECTS

vec2 ParallaxMapping(sampler2D _depthMap, vec2 _texCoords, vec3 _viewDir, float _heightScale) { 
    float height = texture(_depthMap, _texCoords).r;     
    return _texCoords - _viewDir.xy / _viewDir.z * (height * _heightScale);        
}

vec2 SteepParallaxMapping(sampler2D _depthMap, vec2 _texCoords, vec3 _viewDir, float _heightScale, int _minLayers, int _maxLayers) { 
    float numLayers = mix(_maxLayers, _minLayers, abs(dot(vec3(0.0, 0.0, 1.0), _viewDir)));  
    float layerDepth = 1.0 / numLayers;
    
    float currentLayerDepth = 0.0;
    vec2 deltaTexCoords = (_viewDir.xy / _viewDir.z * _heightScale) / numLayers;// applied per layer
    
    vec2 currentTexCoords = _texCoords;
    float currentDepthMapValue = texture(_depthMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue) {
        currentDepthMapValue = texture(_depthMap, currentTexCoords -= deltaTexCoords).r;  
        currentLayerDepth += layerDepth;// next layer
    }

    return currentTexCoords;
}

vec2 ParallaxOcclusionMapping(sampler2D _depthMap, vec2 _texCoords, vec3 _viewDir, float _heightScale, int _minLayers, int _maxLayers) { 
    float numLayers = mix(_maxLayers, _minLayers, abs(dot(vec3(0.0, 0.0, 1.0), _viewDir)));  
    float layerDepth = 1.0 / numLayers;
    
    float currentLayerDepth = 0.0;
    vec2 deltaTexCoords = (_viewDir.xy / _viewDir.z * _heightScale) / numLayers;// applied per layer
    
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

#endif //!CCKIT_GLSL_EFFECTS