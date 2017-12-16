#ifndef CCKIT_UTILS_H
#define CCKIT_UTILS_H

#include <glad/glad.h>

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

	GLuint TextureFromFile(const char* _fileRelativePath, const std::string& _directory) {
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
}

#endif // !CCKIT_UTILS_H