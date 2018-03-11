#ifndef CCKIT_GROUND_H
#define CCKIT_GROUND_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLbehavior.h"

namespace cckit
{
	class BehaviorGround : public GLbehavior
	{
	private:
		typedef BehaviorGround this_type;
	public:
		BehaviorGround(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() {
			mpObj->set_position(glm::vec3(0, -1, 0));
			//mpObj->mRotation = glm::vec3(90, 0, 0);
			mpObj->mScale = glm::vec3(5.0);
			GLrenderer& rend = *mpObj->renderer_ptr();
			rend.mDiffuseColor = glm::vec3(0, 0, 0.3);
			rend.mSpecularColor = glm::vec3(1);
			rend.mShininess = 32;
		}

		void update(float _deltaTime) {

		}
	};
}

#endif // !CCKIT_GROUND_H