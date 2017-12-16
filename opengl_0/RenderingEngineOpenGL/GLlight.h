#ifndef CCKIT_GLLIGHT_H
#define CCKIT_GLLIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace cckit
{
	struct GLlight {
		glm::vec3 mPos;
		glm::vec3 mColor;
		GLlight() = default;
		GLlight(const glm::vec3& _pos, const glm::vec3 _color) 
			: mPos(_pos), mColor(_color)
		{}
	};
}

#endif // !CCKIT_GLLIGHT_H
