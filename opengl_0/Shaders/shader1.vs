#version 330 core

layout(location = 0) in vec3 aLocalPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform mat4 normalModelMat;

void main()
{
	vec4 worldPos = modelMat * vec4(aLocalPos, 1.0);
	
	texCoord = aTexCoord;
	fragPos = vec3(worldPos);
	normal = normalize(mat3(normalModelMat) * aNormal);

	gl_Position = projectionMat * viewMat * worldPos;
}
