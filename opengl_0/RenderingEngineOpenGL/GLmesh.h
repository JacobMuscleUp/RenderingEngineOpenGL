#ifndef CCKIT_MESH_H
#define CCKIT_MESH_H

#include <glad/glad.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "GLshader.h"

#define DIFFUSE_TEXTURE_IN_SHADER "diffuseMap"
#define SPECULAR_TEXTURE_IN_SHADER "specularMap"
#define NORMAL_TEXTURE_IN_SHADER "normalMap"
#define HEIGHT_TEXTURE_IN_SHADER "heightMap"

namespace cckit 
{
	struct GLvertex
	{
		glm::vec3 mPos, mNormal, mTangent, mBitangent;
		glm::vec2 mTexCoords;
		GLvertex() = default;
		GLvertex(const glm::vec3& _pos, const glm::vec3& _normal, const glm::vec2& _texCoords
		, const glm::vec3& _tangent = glm::vec3(), const glm::vec3& _bitangent = glm::vec3())
			: mPos(_pos), mNormal(_normal), mTexCoords(_texCoords), mTangent(_tangent), mBitangent(_bitangent)
		{}
	};

	struct GLtexture
	{
		GLuint mHandle;
		std::string mType;
		std::string mPath;
		GLtexture() = default;
		GLtexture(GLuint _handle, const std::string& _type, const std::string& _path)
			: mHandle(_handle), mType(_type), mPath(_path)
		{}
	};

	class GLmesh 
	{
	public:
		GLmesh(const std::vector<GLvertex>& _vertices
			, const std::vector<GLuint>& _indices
			, const std::vector<GLtexture>& _textures)
			: mVertices(_vertices), mIndices(_indices), mTextures(_textures) {
			Setup();
		}
		~GLmesh();
		void render(const GLshader& _shader) const;
	private:
		void Setup();
	public:
		std::vector<GLvertex> mVertices;// for configuring vertex attributes
		std::vector<GLuint> mIndices;
		std::vector<GLtexture> mTextures;
	private:
		GLuint mVaoHandle;
	};

	GLmesh::~GLmesh() {
		glDeleteVertexArrays(1, &mVaoHandle);
	}

	void GLmesh::render(const GLshader& _shader) const {
		GLuint diffuseIndex = 0
			, specularIndex = 0
			, normalIndex = 0
			, heightIndex = 0;
		for (GLuint i = 0; i < mTextures.size(); ++i) {
			glActiveTexture(GL_TEXTURE0 + i);

			std::string textureType = mTextures[i].mType
				, textureIndex;

			if (textureType == DIFFUSE_TEXTURE_IN_SHADER)
				textureIndex = std::to_string(++diffuseIndex);
			else if (textureType == SPECULAR_TEXTURE_IN_SHADER)
				textureIndex = std::to_string(++specularIndex);
			else if (textureType == NORMAL_TEXTURE_IN_SHADER)
				textureIndex = std::to_string(++normalIndex);
			else if (textureType == HEIGHT_TEXTURE_IN_SHADER)
				textureIndex = std::to_string(++heightIndex);

			_shader.set1i(("material" + textureIndex + "." + textureType).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, mTextures[i].mHandle);
		}

		glBindVertexArray(mVaoHandle);
		glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

	void GLmesh::Setup() {
		GLuint vboHandle, eboHandle;

		glGenVertexArrays(1, &mVaoHandle);
		glGenBuffers(1, &vboHandle);
		glGenBuffers(1, &eboHandle);

		glBindVertexArray(mVaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);

		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(GLvertex), &mVertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), &mIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), reinterpret_cast<void*>(offsetof(GLvertex, mNormal)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLvertex), reinterpret_cast<void*>(offsetof(GLvertex, mTexCoords)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), reinterpret_cast<void*>(offsetof(GLvertex, mTangent)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), reinterpret_cast<void*>(offsetof(GLvertex, mBitangent)));

		glBindVertexArray(0);
		glDeleteBuffers(1, &vboHandle);
		glDeleteBuffers(1, &eboHandle);
	}
}

#endif // !CCKIT_MESH_H
