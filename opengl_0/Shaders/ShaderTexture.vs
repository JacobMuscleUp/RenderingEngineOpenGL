#version 330 core

layout(location = 0) in vec3 aLocalPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
} vs_out;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matNormalModel;

void main()
{
	vec4 worldPos = matModel * vec4(aLocalPos, 1.0);
	
	vs_out.texCoords = aTexCoords;
	vs_out.fragPos = vec3(worldPos);
	vs_out.normal = normalize(mat3(matNormalModel) * aNormal);

	gl_Position = matProjection * matView * worldPos;
}
