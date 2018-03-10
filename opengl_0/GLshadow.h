#ifndef GL_SHADOW_H
#define GL_SHADOW_H

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace cckit 
{
	class GLshadow {
	public:
		GLshadow(GLuint _width, GLuint _height);
		void render(const glm::vec3& _lightPos);
	private:
		GLuint mDepthMapFBOHandle;
	};

	GLshadow::GLshadow(GLuint _width, GLuint _height) {
		GLuint depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenFramebuffers(1, &mDepthMapFBOHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBOHandle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

#endif // !GL_SHADOW_H
