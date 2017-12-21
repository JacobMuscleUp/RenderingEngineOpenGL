#ifndef CCKIT_GL_RENDERER_H
#define CCKIT_GL_RENDERER_H

#include <glm/glm.hpp>
#include "GLmodel.h"

namespace cckit
{
	class GLobj;

	class GLrenderer
	{
		GLrenderer(std::function<void(std::vector<GLvertex>&, std::vector<GLuint>&, std::vector<GLtexture>&)> _config)
			: mpModel(new GLmodel(_config))
		{}
		GLrenderer(const std::string& _modelPath)
			: mpModel(new GLmodel(_modelPath))
		{}
		~GLrenderer() {
			delete mpModel;
		}
	private:
		GLmodel* mpModel;
	public:
		glm::vec3 mDiffuseColor, mSpecularColor;
		int mShininess;

		friend GLobj;
	};
}

#endif // !CCKIT_GL_RENDERER_H