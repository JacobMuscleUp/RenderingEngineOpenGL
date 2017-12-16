#ifndef CCKIT_BULL_H
#define CCKIT_BULL_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLobj.h"
#include "../RenderingEngineOpenGL/GLbehavior.h"
#include "../global.h"

namespace cckit
{
	class bull : public GLbehavior
	{
	private:
		typedef bull this_type;
	public:
		bull(std::function<void(this_type&)> _initConfig = [](this_type&) {}) { _initConfig(*this); }

		void start() const {
			mpObj->mlocalRotation = glm::vec3(0, 180, 0);
			mpObj->mbCoordAxesDrawn = true;
			mpObj->mbOutlined = true;
			mpObj->mOutlineScale = 1.1f;
			mpObj->mOutlineColor = glm::vec3(1, 1, 0);

			mpObj->set_position(glm::vec3(0, 0, -1));
			mpObj->mRotation = glm::vec3(0, 270, 0);
			mpObj->mScale = glm::vec3(0.1f);
		}

		void update(float _deltaTime) const {
			mpObj->mRotateFunc = [this](glm::mat4& _modelMat) {
				_modelMat = glm::rotate(_modelMat, glm::radians((float)glfwGetTime() * 20), glm::vec3(0, 1, 0));
				//_modelMat = glm::lookAt(obj().position(), camera.pos(), glm::vec3(0, 1, 0));
			};
			mpObj->set_position(mpObj->position() + mpObj->forward() * _deltaTime * 0.1f);
		}
	};
}

#endif // !CCKIT_BULL_H