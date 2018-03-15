#ifndef CCKIT_UTILS_H
#define CCKIT_UTILS_H

#include <glad/glad.h>
#include "GLbase.h"
#include "GLmatrixTransform.h"

namespace cckit
{
	/*template<size_t Size, typename T>
	inline constexpr size_t length(T(&_arr)[Size]) {
		return Size;
	}*/

	template<typename NumberType>
	constexpr NumberType min(NumberType a, NumberType b) {
		return (a < b) ? a : b;
	}
	template<typename NumberType>
	constexpr NumberType max(NumberType a, NumberType b) {
		return (b < a) ? a : b;
	}

	glm::vec3 operator*(const glm::mat4& _mat, const glm::vec3& _vec) {
		return glm::vec3(
			_mat[0][0] * _vec[0] + _mat[1][0] * _vec[1] + _mat[2][0] * _vec[2] + _mat[3][0]
			, _mat[0][1] * _vec[0] + _mat[1][1] * _vec[1] + _mat[2][1] * _vec[2] + _mat[3][1]
			, _mat[0][2] * _vec[0] + _mat[1][2] * _vec[1] + _mat[2][2] * _vec[2] + _mat[3][2]
		);
	}

	GLuint load_texture(const char* _fileRelativePath, const std::string& _directory) {
		std::string filePath = _fileRelativePath;
		filePath = _directory + '/' + filePath;

		GLuint textureHandle;
		glGenTextures(1, &textureHandle);

		int width, height, numComponents;
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &numComponents, 0);
		if (data) {
			GLenum format;
			if (numComponents == 1)
				format = GL_RED;
			else if (numComponents == 3)
				format = GL_RGB;
			else if (numComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureHandle);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "Texture failed to load at path: " << _fileRelativePath << std::endl;
		}
		stbi_image_free(data);

		return textureHandle;
	}

	std::string glLoadFile(const std::string& _path) {
		std::string fileStr;
		std::ifstream fileIfstream;
		fileIfstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			fileIfstream.open(_path);

			std::stringstream fileSstream;
			fileSstream << fileIfstream.rdbuf();
			fileIfstream.close();

			fileStr = fileSstream.str();
		}
		catch (std::ifstream::failure _exception) {
			std::cout << "input file stream failure" << "\n";
			return "";
		}

		return fileStr;
	}

	void destroy(const GLbase& _base) {
		(const_cast<GLbase&>(_base)).destroy();
	}

#pragma region GLframebuffer
	class GLframebuffer
	{
	public:
		GLframebuffer(int _screenWidth, int _screenHeight);
		~GLframebuffer() {
			glDeleteFramebuffers(1, &mFboHandle);
			glDeleteTextures(1, &mTextureColorHandle);
			glDeleteRenderbuffers(1, &mRboDepthStencilHandle);
		}
		template<bool Active>
		void set_active() {
			glBindFramebuffer(GL_FRAMEBUFFER, mFboHandle);
		}
		template<>
		void set_active<false>() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void push_texture(GLenum _texture);
	private:
		GLuint mFboHandle, mTextureColorHandle, mRboDepthStencilHandle;
	};
	GLframebuffer::GLframebuffer(int _screenWidth, int _screenHeight) {
		glGenFramebuffers(1, &mFboHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, mFboHandle);
		// color attachment
		glGenTextures(1, &mTextureColorHandle);
		glBindTexture(GL_TEXTURE_2D, mTextureColorHandle);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _screenWidth, _screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _screenWidth, _screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);// for HDR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorHandle, 0);
		// depth and stencil attachment
		glGenRenderbuffers(1, &mRboDepthStencilHandle);
		glBindRenderbuffer(GL_RENDERBUFFER, mRboDepthStencilHandle);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _screenWidth, _screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRboDepthStencilHandle);
		// verify framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "framebuffer incomplete" << "\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void GLframebuffer::push_texture(GLenum _texture) {
		glActiveTexture(_texture);
		glBindTexture(GL_TEXTURE_2D, mTextureColorHandle);
	}
#pragma endregion GLframebuffer

#pragma region GLframebufferDepthMap
	class GLframebufferDepthMap
	{
	public:
		GLframebufferDepthMap(GLuint _width, GLuint _height);
		~GLframebufferDepthMap() {
			glDeleteFramebuffers(1, &mFboHandle);
			glDeleteTextures(1, &mDepthMapHandle);
		}
		template<bool Active>
		void set_active() {
			glBindFramebuffer(GL_FRAMEBUFFER, mFboHandle);
		}
		template<>
		void set_active<false>() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void push_depthmap(GLenum _texture);

		glm::mat4 GetDepthMapSpaceMatrix(const glm::vec3& _vOriginPos, const glm::vec3& _vDir, float _nearPlane, float _farPlane) const;
	private:
		GLuint mFboHandle, mDepthMapHandle;
	};
	GLframebufferDepthMap::GLframebufferDepthMap(GLuint _width, GLuint _height) {
		glGenTextures(1, &mDepthMapHandle);
		glBindTexture(GL_TEXTURE_2D, mDepthMapHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenFramebuffers(1, &mFboHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, mFboHandle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMapHandle, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void GLframebufferDepthMap::push_depthmap(GLenum _texture) {
		glActiveTexture(_texture);
		glBindTexture(GL_TEXTURE_2D, mDepthMapHandle);
	}
	glm::mat4 GLframebufferDepthMap::GetDepthMapSpaceMatrix(
		const glm::vec3& _vOriginPos, const glm::vec3& _vDir, float _nearPlane, float _farPlane) const {
		glm::mat4 matProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, _nearPlane, _farPlane);
		glm::mat4 matView = glm::lookAtRH(_vOriginPos, _vOriginPos + _vDir);
		return matProjection * matView;
	}
#pragma endregion GLframebufferDepthMap

#pragma region GLquad
	class GLquad
	{
	public:
		GLquad();
		~GLquad() {
			glDeleteVertexArrays(1, &mVaoHandle);
			glDeleteBuffers(1, &mVboHandle);
		}
		void prepare() const;
		void render(GLenum _drawMode) const;
	private:
		GLuint mVaoHandle, mVboHandle;
	};
	GLquad::GLquad() {
		GLfloat quadVertices[] = {// vertex : texCoord <=> 2 : 2
			-1.0f, 1.0f, 0.0f, 1.0f
			, -1.0f, -1.0f, 0.0f, 0.0f
			, 1.0f, -1.0f, 1.0f, 0.0f
			,
			-1.0f, 1.0f, 0.0f, 1.0f
			, 1.0f, -1.0f, 1.0f, 0.0f
			, 1.0f, 1.0f, 1.0f, 1.0f
		};

		glGenVertexArrays(1, &mVaoHandle);
		glGenBuffers(1, &mVboHandle);
		glBindVertexArray(mVaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, mVboHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
	}
	void GLquad::prepare() const {
		glBindVertexArray(mVaoHandle);
	}
	void GLquad::render(GLenum _drawMode) const {
		glPolygonMode(GL_FRONT_AND_BACK, _drawMode);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
#pragma endregion GLquad

	GLuint glLoadCubeMap(const std::vector<std::string>& _faces) {
		GLuint textureHandle;
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
		
		int width, height, numChannels;
		for (size_t i = 0; i < _faces.size(); ++i) {
			void* pData = stbi_load(_faces[i].c_str(), &width, &height, &numChannels, 0);
			if (pData) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
			}
			stbi_image_free(pData);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureHandle;
	}

	void glQueryTangentSpace(const glm::vec3& _p0, const glm::vec3& _p1, const glm::vec3& _p2
		, const glm::vec2& _uv0, const glm::vec2& _uv1, const glm::vec2& _uv2
		, glm::vec3& _tangent, glm::vec3& _bitangent) {
		glm::vec3 deltaP0 = _p1 - _p0
			, deltaP1 = _p2 - _p0;
		glm::vec2 deltaUV0 = _uv1 - _uv0
			, deltaUV1 = _uv2 - _uv0;

		float scale = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);
		_tangent.x = scale * (deltaUV1.y * deltaP0.x - deltaUV0.y * deltaP1.x);
		_tangent.y = scale * (deltaUV1.y * deltaP0.y - deltaUV0.y * deltaP1.y);
		_tangent.z = scale * (deltaUV1.y * deltaP0.z - deltaUV0.y * deltaP1.z);
		_bitangent.x = scale * (-deltaUV1.x * deltaP0.x + deltaUV0.x * deltaP1.x);
		_bitangent.y = scale * (-deltaUV1.x * deltaP0.y + deltaUV0.x * deltaP1.y);
		_bitangent.z = scale * (-deltaUV1.x * deltaP0.z + deltaUV0.x * deltaP1.z);
		_tangent = glm::normalize(_tangent);
		_bitangent = glm::normalize(_bitangent);
	}
}

#endif // !CCKIT_UTILS_H