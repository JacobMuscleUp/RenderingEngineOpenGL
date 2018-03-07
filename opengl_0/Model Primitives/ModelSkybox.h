#ifndef CCKIT_MODEL_SKYBOX_H
#define CCKIT_MODEL_SKYBOX_H

#include "../RenderingEngineOpenGL/GLmesh.h"

namespace cckit
{
	auto modelSkyboxConfig
		=
		[](std::vector<cckit::GLvertex>& _vertices, std::vector<GLuint>& _indices, std::vector<cckit::GLtexture>& _textures) {
		GLuint indices[36];// 6 X 6 <=> faces X faceIndices
		for (size_t i = 0, iEnd = cckit::length(indices) / 6; i < iEnd; ++i) {
			size_t n0 = 6 * i, n1 = 4 * i;
			indices[n0] = n1;
			indices[n0 + 1] = n1 + 3;
			indices[n0 + 2] = n1 + 1;
			indices[n0 + 3] = n1 + 3;
			indices[n0 + 4] = n1 + 2;
			indices[n0 + 5] = n1 + 1;
		}
		_vertices.reserve(24);// 6 X 4 <=> faces X faceVerts
		// front
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, 1, 1), glm::vec3(0, 0, -1), glm::vec2(-1, 1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, -1, 1), glm::vec3(0, 0, -1), glm::vec2(-1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, -1, 1), glm::vec3(0, 0, -1), glm::vec2(1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, 1, 1), glm::vec3(0, 0, -1), glm::vec2(1, 1)));
		// back
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, 1, -1), glm::vec3(0, 0, 1), glm::vec2(1, 1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, -1, -1), glm::vec3(0, 0, 1), glm::vec2(1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, -1, -1), glm::vec3(0, 0, 1), glm::vec2(-1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, 1, -1), glm::vec3(0, 0, 1), glm::vec2(-1, 1)));
		// top
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, 1, 1), glm::vec3(0, -1, 0), glm::vec2(1, 1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, 1, -1), glm::vec3(0, -1, 0), glm::vec2(1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, 1, -1), glm::vec3(0, -1, 0), glm::vec2(-1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, 1, 1), glm::vec3(0, -1, 0), glm::vec2(-1, 1)));
		// bottom
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, -1, 1), glm::vec3(0, 1, 0), glm::vec2(-1, 1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, -1, -1), glm::vec3(0, 1, 0), glm::vec2(-1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, -1, -1), glm::vec3(0, 1, 0), glm::vec2(1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, -1, 1), glm::vec3(0, 1, 0), glm::vec2(1, 1)));
		// right
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(1, 1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, -1, 1), glm::vec3(-1, 0, 0), glm::vec2(1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, -1, -1), glm::vec3(-1, 0, 0), glm::vec2(-1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(1, 1, -1), glm::vec3(-1, 0, 0), glm::vec2(-1, 1)));
		// left
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, 1, -1), glm::vec3(1, 0, 0), glm::vec2(-1, 1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, -1, -1), glm::vec3(1, 0, 0), glm::vec2(-1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, -1, 1), glm::vec3(1, 0, 0), glm::vec2(1, -1)));
		_vertices.push_back(cckit::GLvertex(glm::vec3(-1, 1, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1)));

		for (size_t i = 0, iEnd = cckit::length(indices); i < iEnd; ++i)
			_indices.push_back(indices[i]);
	};
}

#endif // !CCKIT_MODEL_SKYBOX_H