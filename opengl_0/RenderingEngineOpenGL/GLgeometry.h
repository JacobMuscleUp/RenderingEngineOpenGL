#ifndef CCKIT_GL_GEOMETRY_H
#define CCKIT_GL_GEOMETRY_H

#include "GLutils.h"
#include "GLshader.h"

namespace cckit
{
	enum class EnumPrimitive { line = 0, count };

	class GLprimitive
	{
	public:
		GLprimitive(EnumPrimitive _type) : mType(_type) {}
		virtual void render(const GLshader&, std::function<void(const GLshader&)> _uniformConfig) const = 0;
		EnumPrimitive type() const { return mType; }
	protected:
		EnumPrimitive mType;
	};

	class GLline : public GLprimitive
	{
	public:
		GLline() : GLprimitive(EnumPrimitive::line) {}
		GLline(const glm::vec3& _from, const glm::vec3& _to, const glm::vec3& _color);
		void render(const GLshader&, std::function<void(const GLshader&)> _uniformConfig) const;
		void set(const glm::vec3& _new, char _flag = 0x3);
	private:
		glm::vec3 mFrom, mTo;
		glm::vec3 mColor;
	};
	GLline::GLline(const glm::vec3& _from, const glm::vec3& _to, const glm::vec3& _color)
		: mFrom(_from), mTo(_to), mColor(_color), GLprimitive(EnumPrimitive::line)
	{}
	void GLline::render(const GLshader& _shader, std::function<void(const GLshader&)> _uniformConfig) const {
		glm::vec3 vertices3D[] = { mFrom, mTo, mFrom };
		GLfloat vertices1D[18];
		for (size_t i = 0; i < length(vertices3D); ++i) {
			size_t i0 = 6 * i;
			vertices1D[i0] = vertices3D[i][0];
			vertices1D[i0 + 1] = vertices3D[i][1];
			vertices1D[i0 + 2] = vertices3D[i][2];
			vertices1D[i0 + 3] = mColor[0];
			vertices1D[i0 + 4] = mColor[1];
			vertices1D[i0 + 5] = mColor[2];
		}

		_uniformConfig(_shader);

		GLuint vaoHandle, vboHandle;
		glGenVertexArrays(1, &vaoHandle);
		glGenBuffers(1, &vboHandle);

		glBindVertexArray(vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1D), vertices1D, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, length(vertices3D));

		glDeleteVertexArrays(1, &vaoHandle);
		glDeleteBuffers(1, &vboHandle);
	}
	void GLline::set(const glm::vec3& _new, char _flag) {
		if (_flag & 0x1)
			mFrom = _new;
		if (_flag & 0x2)
			mTo = _new;
		if (_flag & 0x4)
			mColor = _new;
	}
}

#endif // !CCKIT_GL_GEOMETRY_H