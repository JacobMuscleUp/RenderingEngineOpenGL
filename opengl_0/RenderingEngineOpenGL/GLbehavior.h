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
		virtual void start() const {}
		virtual void update(float _deltaTime) const {}
		GLobj& obj() const { return *mpObj; }
	protected:
		mutable GLobj* mpObj;

		friend GLobj;
	};
}

#endif // !CCKIT_GL_BEHAVIOR_H
