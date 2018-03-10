#ifndef CCKIT_GLSHADER_H
#define CCKIT_GLSHADER_H

#include <glad/glad.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <forward_list>
#include <utility>
#include "GLfactory.h"

namespace cckit
{
	template<size_t Size, typename T>
	inline constexpr size_t length(T(&_arr)[Size]) {
		return Size;
	}

	class GLrenderer;

	class GLshader
	{
		friend GLfactory<GLshader>;
	private:
		GLshader() : mValid(GL_FALSE) { mInstances.push_front(this); }
	public:
		void load(const GLchar* _vsPath, const GLchar* _fsPath);
	
		void use() const
		{
			if (mValid)
				glUseProgram(mHandle);
		}

		void set1f(const GLchar* _uniformVar, GLfloat _arg0) const {
			glUniform1f(glGetUniformLocation(mHandle, _uniformVar), _arg0);
		}
		void set3f(const GLchar* _uniformVar, GLfloat _arg0, GLfloat _arg1, GLfloat _arg2) const {
			glUniform3f(glGetUniformLocation(mHandle, _uniformVar), _arg0, _arg1, _arg2);
		}
		void set4f(const GLchar* _uniformVar, GLfloat _arg0, GLfloat _arg1, GLfloat _arg2, GLfloat _arg3) const {
			glUniform4f(glGetUniformLocation(mHandle, _uniformVar), _arg0, _arg1, _arg2, _arg3);
		}
		void set1i(const GLchar* _uniformVar, GLint _arg0) const {
			glUniform1i(glGetUniformLocation(mHandle, _uniformVar), _arg0);
		}
		void setmatrix4fv(const GLchar* _uniformVar, GLsizei _count, GLboolean _transpose, const GLfloat* _value) const {
			glUniformMatrix4fv(glGetUniformLocation(mHandle, _uniformVar), _count, _transpose, _value);
		}
		void set3fv(const GLchar* _uniformVar, const GLfloat* _arg0) const {
			glUniform3fv(glGetUniformLocation(mHandle, _uniformVar), 1, _arg0);
		}

		GLuint get_handle() const { return mHandle; }
		const std::string& vs_path() const { return mVsPath; }
		const std::string& fs_path() const { return mFsPath; }

		static void unload() {
			while (!mInstances.empty()) {
				delete mInstances.front();
				mInstances.pop_front();
			}
		}

	private:
		GLuint mHandle;
		GLboolean mValid;
		std::string mVsPath, mFsPath;
	public:
		std::function<void(const GLshader&)> mFsGlobalConfig;// the GLshader arg must be currently used prior to its invocation
		std::function<void(const GLshader&, const GLrenderer&)> mFsLocalConfig;// the GLshader arg must be currently used prior to its invocation
	private:
		static std::forward_list<const GLshader*> mInstances;
	public:
		static std::unordered_map<
			size_t
			, std::unordered_map<
				size_t
				, std::pair<std::function<void(const GLshader&)>, std::function<void(const GLshader&, const GLrenderer&)> > > > mMapShaderPath2FsGLConfig;
		static std::hash<std::string> mStringHash;
	};
	std::forward_list<const GLshader*> GLshader::mInstances;
	std::unordered_map<
		size_t
		, std::unordered_map<
		size_t
		, std::pair<std::function<void(const GLshader&)>, std::function<void(const GLshader&, const GLrenderer&)> > > > GLshader::mMapShaderPath2FsGLConfig;
	std::hash<std::string> GLshader::mStringHash;

	void GLshader::load(const GLchar* _vsPath, const GLchar* _fsPath) {
		mValid = GL_TRUE;
		mVsPath = _vsPath;
		mFsPath = _fsPath;

		std::string vertexShaderSourceStr, fragmentShaderSourceStr;
		std::ifstream vertexShaderIfstream, fragmentShaderIfstream;
		vertexShaderIfstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vertexShaderIfstream.open(_vsPath);
			fragmentShaderIfstream.open(_fsPath);

			std::stringstream vertexShaderSstream, fragmentShaderSstream;
			vertexShaderSstream << vertexShaderIfstream.rdbuf();
			fragmentShaderSstream << fragmentShaderIfstream.rdbuf();
			vertexShaderIfstream.close();
			fragmentShaderIfstream.close();

			vertexShaderSourceStr = vertexShaderSstream.str();
			fragmentShaderSourceStr = fragmentShaderSstream.str();
		}
		catch (std::ifstream::failure _exception) {
			std::cout << "input file stream failure" << "\n";
			return;
		}

		const GLchar *vertexShaderSource = vertexShaderSourceStr.c_str()
			, *fragmentShaderSource = fragmentShaderSourceStr.c_str();

		const GLuint vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER)
			, fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertexShaderHandle, 1, &vertexShaderSource, nullptr);
		glShaderSource(fragmentShaderHandle, 1, &fragmentShaderSource, nullptr);
		glCompileShader(vertexShaderHandle);
		glCompileShader(fragmentShaderHandle);

		GLint bShaderCompiled = GL_TRUE;
		glGetShaderiv(vertexShaderHandle, GL_COMPILE_STATUS, &bShaderCompiled);
		if (!bShaderCompiled) {
			char infoLog[512];
			glGetShaderInfoLog(vertexShaderHandle, length(infoLog), nullptr, infoLog);
			std::cout << "vertex shader compilation failed\n" << infoLog << "\n";
			mValid = GL_FALSE;
			return;
		}
		glGetShaderiv(fragmentShaderHandle, GL_COMPILE_STATUS, &bShaderCompiled);
		if (!bShaderCompiled) {
			char infoLog[512];
			glGetShaderInfoLog(fragmentShaderHandle, length(infoLog), nullptr, infoLog);
			std::cout << "fragment shader compilation failed\n" << infoLog << "\n";
			mValid = GL_FALSE;
			return;
		}

		mHandle = glCreateProgram();
		glAttachShader(mHandle, vertexShaderHandle);
		glAttachShader(mHandle, fragmentShaderHandle);
		glLinkProgram(mHandle);
		glDetachShader(mHandle, vertexShaderHandle);
		glDetachShader(mHandle, fragmentShaderHandle);
		glDeleteShader(vertexShaderHandle);
		glDeleteShader(fragmentShaderHandle);

		GLint bProgramLinked = GL_TRUE;
		glGetProgramiv(mHandle, GL_LINK_STATUS, &bProgramLinked);
		if (!bProgramLinked) {
			char infoLog[512];
			glGetProgramInfoLog(mHandle, length(infoLog), nullptr, infoLog);
			std::cout << "program linking failed\n" << infoLog << "\n";
			mValid = GL_FALSE;
			return;
		}
	}
}

#endif // !CCKIT_GLSHADER_H
