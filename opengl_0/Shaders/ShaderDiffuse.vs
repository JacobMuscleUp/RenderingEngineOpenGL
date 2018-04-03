#version 330 core

layout(location = 0) in vec3 aLocalPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
	vec3 fragPosLightSpace;////
} vs_out;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform mat4 normalModelMat;

uniform mat4 matLightSpace;////

void main()
{
	vec4 worldPos = modelMat * vec4(aLocalPos, 1.0);
	vec4 worldPosLightSpace = projectionMat * matLightSpace * worldPos;////
	
	vs_out.texCoords = aTexCoords;
	vs_out.fragPos = vec3(worldPos);
	vs_out.normal = normalize(mat3(normalModelMat) * aNormal);
	vs_out.fragPosLightSpace = worldPosLightSpace.xyz / worldPosLightSpace.w;////

	gl_Position = projectionMat * viewMat * worldPos;
}
