#ifndef CCKIT_BULL_SPAWNER_H
#define CCKIT_BULL_SPAWNER_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLobj.h"
#include "../RenderingEngineOpenGL/GLbehavior.h"
#include "../global.h"
#include "../RenderingEngineOpenGL/GLmatrixTransform.h"
#include "../RenderingEngineOpenGL/GLfactory.h"
#include <iostream>

#define CONFIG_BULL_SPAWN_TIME 3.0f

namespace cckit
{
	class bull_spawner : public GLbehavior
	{
	private:
		typedef bull_spawner this_type;
	public:
		bull_spawner(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() {
			mSpawnTime = CONFIG_BULL_SPAWN_TIME;
			mSpawnTimer = 0.0f;
		}

		void update(float _deltaTime) {
			if ((mSpawnTimer += _deltaTime) > mSpawnTime) {
				mSpawnTimer = 0.0f;
				cckit::GLobj& bull = *cckit::GLfactory<cckit::GLobj>::generate();
				bull.add_behavior(new cckit::bull);
				bull.load_model("Resources/OBJ/bull/bull.obj");
				bull.set_shader(*pShaderDiffuse);
			}
		}

		void on_destroyed() {
			std::cout << "spawner destroyed" << "\n";
		}
	public:
		float mSpawnTime;
	private:
		float mSpawnTimer;
	};
}

#endif // !CCKIT_BULL_SPAWNER_H