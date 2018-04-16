#version 330 core

layout(location = 0) in vec3 aLocalPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT {
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
	mat3 matTBN;
	vec3 fragPosLightSpaceNDC;
} vs_out;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matNormalModel;
uniform mat4 matProjectionLightSpace;

uniform mat4 matViewLightSpace;

void main()
{
	vec4 worldPos = matModel * vec4(aLocalPos, 1.0);
	vec4 fragPosLightSpaceClip = matProjectionLightSpace * matViewLightSpace * worldPos;
	
	vs_out.texCoords = aTexCoords;
	vs_out.fragPos = vec3(worldPos);
	vs_out.matTBN = mat3(aTangent, aBitangent, aNormal);
	vs_out.normal = normalize(mat3(matNormalModel) * aNormal);
	vs_out.fragPosLightSpaceNDC = fragPosLightSpaceClip.xyz / fragPosLightSpaceClip.w;

	gl_Position = matProjection * matView * worldPos;
}