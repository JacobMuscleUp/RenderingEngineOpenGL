#ifndef CCKIT_GL_BEHAVIOR_H
#define CCKIT_GL_BEHAVIOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include "GLbase.h"
#include "GLobj.h"

namespace cckit
{
	// GLobj hasn't been defined yet
	class GLbehavior : public GLbase
	{
	public:
		GLbehavior() : mbStarted(false), mbDestroyed(false), mOnStart2Update([](GLobj&) {}) {}
		void on_start2update() { mbStarted = true; mOnStart2Update(*mpObj); }
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
		std::function<void(GLobj&)> mOnStart2Update;

		friend GLobj;
	};
}

#endif // !CCKIT_GL_BEHAVIOR_H
