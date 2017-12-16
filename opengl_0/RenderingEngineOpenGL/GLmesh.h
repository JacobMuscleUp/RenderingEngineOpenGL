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
		glm::vec3 mPos, mNormal;
		glm::vec2 mTexCoords;
		GLvertex() = default;
		GLvertex(const glm::vec3& _pos, const glm::vec3& _normal, const glm::vec2& _texCoords)
			: mPos(_pos), mNormal(_normal), mTexCoords(_texCoords)
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
		void render(const GLshader& _shader) {
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

	private:
		void Setup() {
			glGenVertexArrays(1, &mVaoHandle);
			glGenBuffers(1, &mVboHandle);
			glGenBuffers(1, &mEboHandle);

			glBindVertexArray(mVaoHandle);
			glBindBuffer(GL_ARRAY_BUFFER, mVboHandle);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboHandle);

			glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(GLvertex), &mVertices[0], GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), &mIndices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), nullptr);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), reinterpret_cast<void*>(offsetof(GLvertex, mNormal)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), reinterpret_cast<void*>(offsetof(GLvertex, mTexCoords)));

			glBindVertexArray(0);
			glDeleteBuffers(1, &mVboHandle);
			glDeleteBuffers(1, &mEboHandle);
		}
	public:
		std::vector<GLvertex> mVertices;
		std::vector<GLuint> mIndices;
		std::vector<GLtexture> mTextures;
	private:
		GLuint mVaoHandle, mVboHandle, mEboHandle;
	};
}

#endif // !CCKIT_MESH_H
