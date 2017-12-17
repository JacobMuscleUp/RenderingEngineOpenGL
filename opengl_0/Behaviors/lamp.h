#ifndef CCKIT_LAMP_H
#define CCKIT_LAMP_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLobj.h"
#include "../RenderingEngineOpenGL/GLbehavior.h"

namespace cckit
{
	class lamp : public GLbehavior
	{
	private:
		typedef lamp this_type;
	public:
		lamp(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() const {
			mpObj->set_position(mStartingPos);
			mpObj->mScale = glm::vec3(0.1f);
		}

		/*void update(float _deltaTime) const {
			
		}*/
	public:
		glm::vec3 mStartingPos;
	};
}

#endif // !CCKIT_LAMP_H