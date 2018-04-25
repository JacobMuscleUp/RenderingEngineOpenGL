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
#include "GLutils.h"

namespace cckit
{
	class GLrenderer;

	class GLshader
	{
		friend GLfactory<GLshader>;
	private:
		GLshader() : mValid(GL_FALSE) { mInstances.push_front(this); }
	public:
		void load(const GLchar* _vsPath, const GLchar* _fsPath, const GLchar* _gsPath = nullptr);
	
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
		void setmatrix4fv(const GLchar* _uniformVar, GLsizei _count, GLboolean _transpose, const glm::mat4& _value) const {
			glUniformMatrix4fv(glGetUniformLocation(mHandle, _uniformVar), _count, _transpose, glm::value_ptr(_value));
		}
		void set3fv(const GLchar* _uniformVar, const glm::vec3& _arg0) const {
			glUniform3fv(glGetUniformLocation(mHandle, _uniformVar), 1, glm::value_ptr(_arg0));
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
		bool Preprocess(std::string& src);
		void PreprocessRec(std::string& src);
		void Load(const GLchar* _vsPath, const GLchar* _fsPath
			, std::function<bool(GLshader&, GLuint&)> _gsInit
			, std::function<void(GLuint)> _gsAttach
			, std::function<void(GLuint)> _gsDetach);

	private:
		GLuint mHandle;
		GLboolean mValid;
		std::string mVsPath, mFsPath, mGsPath;
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

	void GLshader::load(const GLchar* _vsPath, const GLchar* _fsPath, const GLchar* _gsPath) {
		Load(_vsPath, _fsPath
			, [&](GLshader& _shader, GLuint& _gsHandle) {
			if (_gsPath) {
				GLint bShaderCompiled = GL_TRUE;
				mGsPath = _gsPath;

				std::string gsSourceStr = glLoadFile(_gsPath);
				PreprocessRec(gsSourceStr);

				const GLchar *gsSource = gsSourceStr.c_str();

				_gsHandle = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(_gsHandle, 1, &gsSource, nullptr);
				glCompileShader(_gsHandle);

				glGetShaderiv(_gsHandle, GL_COMPILE_STATUS, &bShaderCompiled);
				if (!bShaderCompiled) {
					char infoLog[512];
					glGetShaderInfoLog(_gsHandle, length(infoLog), nullptr, infoLog);
					std::cout << "geometry shader compilation failed\n" << infoLog << "\n";
					mValid = GL_FALSE;
					return false;
				}

				return true;
			}
			return false;
		}, [this](GLuint _gsHandle) {
			glAttachShader(mHandle, _gsHandle);
		}, [this](GLuint _gsHandle) {
			glDetachShader(mHandle, _gsHandle);
			glDeleteShader(_gsHandle);
		});
	}

	bool GLshader::Preprocess(std::string& src) {
		bool bPragmaFound = false;

		std::stringstream sstream(src), sstreamTemp;
		std::string pragma("#pragma include");

		for (std::string line; std::getline(sstream, line);) {
			if (line.find(pragma) != std::string::npos) {
				bPragmaFound = true;
				if (line.find("//") == 0) continue;

				size_t openQuotePos = line.find("\"");
				size_t closingQuotePos = line.find("\"", openQuotePos + 1);

				std::string includedFilePath = line.substr(openQuotePos + 1, closingQuotePos - openQuotePos - 1);
				sstreamTemp << glLoadFile(includedFilePath) << "\n";
			}
			else
				sstreamTemp << line << "\n";
		}
		
		src = sstreamTemp.str();
		return bPragmaFound;
	}

	void GLshader::PreprocessRec(std::string& src) {
		std::stringstream sstream(src), sstreamTemp;
		std::string pragma("#pragma include");

		for (std::string line; std::getline(sstream, line);) {
			if (line.find(pragma) != std::string::npos) {
				if (line.find("//") == 0) continue;

				size_t openQuotePos = line.find("\"");
				size_t closingQuotePos = line.find("\"", openQuotePos + 1);

				std::string includedFilePath = line.substr(openQuotePos + 1, closingQuotePos - openQuotePos - 1);

				std::string includedFileStr = glLoadFile(includedFilePath);
				PreprocessRec(includedFileStr);
				sstreamTemp << includedFileStr << "\n";
			}
			else
				sstreamTemp << line << "\n";
		}

		src = sstreamTemp.str();
	}

	void GLshader::Load(const GLchar* _vsPath, const GLchar* _fsPath
		, std::function<bool(GLshader&, GLuint&)> _gsInit
		, std::function<void(GLuint)> _gsAttach
		, std::function<void(GLuint)> _gsDetach) {
		GLint bShaderCompiled = GL_TRUE, bProgramLinked = GL_TRUE;

		GLuint gsHandle;
		bool bGsExist = _gsInit(*this, gsHandle);

		mValid = GL_TRUE;
		mVsPath = _vsPath;
		mFsPath = _fsPath;

		std::string vsSourceStr = glLoadFile(_vsPath)
			, fsSourceStr = glLoadFile(_fsPath);
		/*Preprocess(vertexShaderSourceStr);
		Preprocess(fragmentShaderSourceStr);
		while (Preprocess(vertexShaderSourceStr)) {}
		while (Preprocess(fragmentShaderSourceStr)) {}*/
		PreprocessRec(vsSourceStr);
		PreprocessRec(fsSourceStr);

		const GLchar *vsSource = vsSourceStr.c_str()
			, *fsSource = fsSourceStr.c_str();

		const GLuint vsHandle = glCreateShader(GL_VERTEX_SHADER)
			, fsHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vsHandle, 1, &vsSource, nullptr);
		glShaderSource(fsHandle, 1, &fsSource, nullptr);
		glCompileShader(vsHandle);
		glCompileShader(fsHandle);

		glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &bShaderCompiled);
		if (!bShaderCompiled) {
			char infoLog[512];
			glGetShaderInfoLog(vsHandle, length(infoLog), nullptr, infoLog);
			std::cout << "vertex shader compilation failed\n" << infoLog << "\n";
			mValid = GL_FALSE;
			return;
		}
		glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &bShaderCompiled);
		if (!bShaderCompiled) {
			char infoLog[512];
			glGetShaderInfoLog(fsHandle, length(infoLog), nullptr, infoLog);
			std::cout << "fragment shader compilation failed\n" << infoLog << "\n";
			mValid = GL_FALSE;
			return;
		}

		mHandle = glCreateProgram();
		glAttachShader(mHandle, vsHandle);
		glAttachShader(mHandle, fsHandle);
		if (bGsExist)
			_gsAttach(gsHandle);
		glLinkProgram(mHandle);
		glDetachShader(mHandle, vsHandle);
		glDetachShader(mHandle, fsHandle);
		glDeleteShader(vsHandle);
		glDeleteShader(fsHandle);
		if (bGsExist)
			_gsDetach(gsHandle);

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
