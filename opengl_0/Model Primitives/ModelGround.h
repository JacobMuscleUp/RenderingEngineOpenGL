#ifndef CCKIT_MODEL_GROUND_H
#define CCKIT_MODEL_GROUND_H

#include "../RenderingEngineOpenGL/GLmesh.h"

namespace cckit
{
	auto modelGroundConfig
		=
		[](std::vector<cckit::GLvertex>& _vertices, std::vector<GLuint>& _indices, std::vector<cckit::GLtexture>& _textures) {
		GLuint indices[6];
		for (size_t i = 0, iEnd = cckit::length(indices) / 6; i < iEnd; ++i) {
			size_t n0 = 6 * i, n1 = 4 * i;
			indices[n0] = n1;
			indices[n0 + 1] = n1 + 1;
			indices[n0 + 2] = n1 + 3;
			indices[n0 + 3] = n1 + 1;
			indices[n0 + 4] = n1 + 2;
			indices[n0 + 5] = n1 + 3;
		}
		_vertices.reserve(4);// 1 X 4 <=> faces X faceVerts

		glm::vec3 p0(1, 1, 1)
			, p1(1, 1, -1)
			, p2(-1, 1, -1)
			, p3(-1, 1, 1);
		glm::vec2 uv0(1, 1)
			, uv1(1, 0)
			, uv2(0, 0)
			, uv3(0, 1);
		glm::vec3 deltaP0 = p1 - p0
			, deltaP1 = p2 - p0;
		glm::vec2 deltaUV0 = uv1 - uv0
			, deltaUV1 = uv2 - uv0;

		float scale = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);
		glm::vec3 tangent(
			scale * (deltaUV1.y * deltaP0.x - deltaUV0.y * deltaP1.x)
			, scale * (deltaUV1.y * deltaP0.y - deltaUV0.y * deltaP1.y)
			, scale * (deltaUV1.y * deltaP0.z - deltaUV0.y * deltaP1.z)
		);
		glm::vec3 bitangent(
			scale * (-deltaUV1.x * deltaP0.x + deltaUV0.x * deltaP1.x)
			, scale * (-deltaUV1.x * deltaP0.y + deltaUV0.x * deltaP1.y)
			, scale * (-deltaUV1.x * deltaP0.z + deltaUV0.x * deltaP1.z)
		);
		tangent = glm::normalize(tangent);
		bitangent = glm::normalize(bitangent);
		
		_vertices.push_back(cckit::GLvertex(p0, glm::vec3(0, 1, 0), uv0, tangent, bitangent));
		_vertices.push_back(cckit::GLvertex(p1, glm::vec3(0, 1, 0), uv1, tangent, bitangent));
		_vertices.push_back(cckit::GLvertex(p2, glm::vec3(0, 1, 0), uv2, tangent, bitangent));
		_vertices.push_back(cckit::GLvertex(p3, glm::vec3(0, 1, 0), uv3, tangent, bitangent));

		for (size_t i = 0, iEnd = cckit::length(indices); i < iEnd; ++i)
			_indices.push_back(indices[i]);

		// populate _textures
		std::string textureDirectory = "Resources/Normal Mapping/brickwall";
		std::string textureRelativePath = "brickwall.jpg";
		std::string texturePath = textureDirectory + "/" + textureRelativePath;
		std::string textureNMRelativePath = "brickwall_normal.jpg";
		std::string textureNMPath = textureDirectory + "/" + textureNMRelativePath;

		GLuint textureHandle = load_texture(textureRelativePath.c_str(), textureDirectory);
		GLuint textureNMHandle = load_texture(textureNMRelativePath.c_str(), textureDirectory);

		_textures.push_back(GLtexture(textureHandle, DIFFUSE_TEXTURE_IN_SHADER, texturePath.c_str()));
		_textures.push_back(GLtexture(textureNMHandle, NORMAL_TEXTURE_IN_SHADER, textureNMPath.c_str()));
	};
}

#endif // !CCKIT_MODEL_GROUND_H