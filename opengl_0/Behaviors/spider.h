#ifndef CCKIT_SPIDER_H
#define CCKIT_SPIDER_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLobj.h"
#include "../RenderingEngineOpenGL/GLbehavior.h"

namespace cckit
{
	class spider : public GLbehavior
	{
	private:
		typedef spider this_type;
	public:
		spider(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() const {
			mpObj->mlocalRotation = glm::vec3(0, 90, 0);
			mpObj->mCoordAxisLength = 2.0f;
			mpObj->mbCoordAxesDrawn = true;
			mpObj->mbOutlined = true;

			mpObj->mRotation = glm::vec3(0, 270, 0);
			mpObj->mScale = glm::vec3(0.01f);
		}

		void update(float _deltaTime) const {
			mpObj->mTranslateFunc = [](glm::mat4& _modelMat) {
				_modelMat = glm::translate(_modelMat, glm::vec3(sin(glfwGetTime()), 0, 0));
			};
			mpObj->mRotateFunc = [](glm::mat4& _modelMat) {
				_modelMat = glm::rotate(_modelMat, glm::radians((float)glfwGetTime() * 10), glm::vec3(2, 3, 1));
			};
			mpObj->mScaleFunc = [](glm::mat4& _modelMat) {
				//_modelMat = glm::scale(_modelMat, glm::vec3(sin(glfwGetTime()) * 0.5f + 1.5f));
			};
			//mpObj->set_position(glm::vec3(sin(glfwGetTime()), mpObj->position()[1], mpObj->position()[2]));
		}
	};
}

#endif // !CCKIT_SPIDER_H