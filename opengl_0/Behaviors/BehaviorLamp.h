#ifndef CCKIT_LAMP_H
#define CCKIT_LAMP_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLbehavior.h"

namespace cckit
{
	class BehaviorLamp : public GLbehavior
	{
	private:
		typedef BehaviorLamp this_type;
	public:
		BehaviorLamp(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() {
			mpObj->set_position(mStartingPos);
			mpObj->mScale = glm::vec3(0.1f);
		}

		void update(float _deltaTime) {
			
		}
	public:
		glm::vec3 mStartingPos;
	};
}

#endif // !CCKIT_LAMP_H