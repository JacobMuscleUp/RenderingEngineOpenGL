#ifndef CCKIT_SPIDER_H
#define CCKIT_SPIDER_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLbehavior.h"

namespace cckit
{
	class BehaviorSpider : public GLbehavior
	{
	private:
		typedef BehaviorSpider this_type;
	public:
		BehaviorSpider(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() {
			mpObj->mlocalRotation = glm::vec3(0, 90, 0);
			mpObj->mCoordAxisLength = 2.0f;
			mpObj->mbCoordAxesDrawn = true;
			mpObj->mbOutlined = true;

			mpObj->mRotation = glm::vec3(0, 270, 0);
			mpObj->mScale = glm::vec3(0.01f);

			if (obj().renderer_ptr()) {
				GLrenderer& rend = *(obj().renderer_ptr());
				rend.mSpecularColor = mSpecularColor;
				rend.mShininess = mShininess;
			}

			mTimeElapsed = 0.0f;
		}

		void update(float _deltaTime) {
			mTimeElapsed += _deltaTime;

			mpObj->mTranslateFunc = [&](glm::mat4& _modelMat) {
				_modelMat = glm::translate(_modelMat, glm::vec3(sin(mTimeElapsed), 0, 0));
			};
			mpObj->mRotateFunc = [&](glm::mat4& _modelMat) {
				_modelMat = glm::rotate(_modelMat, glm::radians(mTimeElapsed * 10), glm::vec3(2, 3, 1));
			};
			mpObj->mScaleFunc = [&](glm::mat4& _modelMat) {
				//_modelMat = glm::scale(_modelMat, glm::vec3(sin(mTimeElapsed) * 0.5f + 1.5f));
			};
			//mpObj->set_position(glm::vec3(sin(glfwGetTime()), mpObj->position()[1], mpObj->position()[2]));
		}
	public:
		glm::vec3 mSpecularColor;
		int mShininess;
	private:
		float mTimeElapsed;
	};
}

#endif // !CCKIT_SPIDER_H