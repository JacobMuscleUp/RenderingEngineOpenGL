#ifndef CCKIT_BULL_H
#define CCKIT_BULL_H

#include "../RenderingEngineOpenGL/GLbehavior.h"
#include "../RenderingEngineOpenGL/GLmatrixTransform.h"
#include "../RenderingEngineOpenGL/GLutils.h"
#include "../global.h"
#include <iostream>

namespace cckit
{
	class BehaviorBull : public GLbehavior
	{
	private:
		typedef BehaviorBull this_type;
	public:
		BehaviorBull(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() {
			mpObj->mlocalRotation = glm::vec3(0, 180, 0);
			mpObj->mbCoordAxesDrawn = true;
			mpObj->mbOutlined = true;
			mpObj->mOutlineScale = 1.1f;
			mpObj->mOutlineColor = glm::vec3(1, 1, 0);
			
			mpObj->set_position(glm::vec3(0, 0, -1));
			mpObj->mRotation = glm::vec3(0, 270, 0);
			mpObj->mScale = glm::vec3(0.1f);
			
			if (obj().renderer_ptr()) {
				GLrenderer& rend = *(obj().renderer_ptr());
				rend.mDiffuseColor = mDiffuseColor;
				rend.mSpecularColor = mSpecularColor;
				rend.mShininess = mShininess;
			}
		}

		void update(float _deltaTime) {
			mpObj->mRotateFunc = [this](glm::mat4& _modelMat) {
				_modelMat = glm::rotate(_modelMat, glm::radians((float)glfwGetTime() * 20), glm::vec3(0, 1, 0));
			};
			mpObj->face(camera.pos(), glm::facing_mode::forward);
			mpObj->set_position(mpObj->position() + mpObj->forward() * _deltaTime * mMoveSpeed);
			
			if (glm::length(obj().position() - camera.pos()) < 0.3f) {
				//cckit::destroy(*this);
				cckit::destroy(obj());
			}
		}

		void on_destroyed() {
			std::cout << "bull destroyed" << "\n";
		}
	public:
		float mMoveSpeed;
		glm::vec3 mDiffuseColor, mSpecularColor;
		int mShininess;
	};
}

#endif // !CCKIT_BULL_H