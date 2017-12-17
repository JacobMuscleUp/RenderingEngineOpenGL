#ifndef CCKIT_GL_BEHAVIOR_H
#define CCKIT_GL_BEHAVIOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace cckit
{
	class GLobj;

	class GLbehavior
	{
	public:
		GLbehavior() : mbStarted(false) {}
		void manage() const { if (!mbStarted) { mbStarted = true; } }
		virtual void start() const {}
		virtual void update(float _deltaTime) const {}
		virtual void on_destroyed() const {}
		GLobj& obj() const { return *mpObj; }
		bool started() const { return mbStarted; }
	protected:
		mutable GLobj* mpObj;
		mutable bool mbStarted;

		friend GLobj;
	};
}

#endif // !CCKIT_GL_BEHAVIOR_H
