#ifndef CCKIT_GL_BEHAVIOR_H
#define CCKIT_GL_BEHAVIOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GLbase.h"

namespace cckit
{
	class GLobj;

	class GLbehavior : public GLbase
	{
	public:
		GLbehavior() : mbStarted(false), mbDestroyed(false) {}
		void manage() { if (!mbStarted) { mbStarted = true; } }
		virtual void start() {}
		virtual void update(float _deltaTime) {}
		virtual void on_destroyed() {}

		void destroy() { mbDestroyed = true; }
		GLobj& obj() const { return *mpObj; }
		bool started() const { return mbStarted; }
	protected:
		GLobj* mpObj;
		bool mbStarted;
		bool mbDestroyed;

		friend GLobj;
	};
}

#endif // !CCKIT_GL_BEHAVIOR_H
