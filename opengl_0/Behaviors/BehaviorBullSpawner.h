#ifndef CCKIT_BULL_SPAWNER_H
#define CCKIT_BULL_SPAWNER_H

#include <GLFW/glfw3.h>
#include "../RenderingEngineOpenGL/GLbehavior.h"
#include "../RenderingEngineOpenGL/GLmatrixTransform.h"
#include "../RenderingEngineOpenGL/GLfactory.h"
#include "../Prefabs/PrefabBull.h"
#include <iostream>

namespace cckit
{
	class BehaviorBullSpawner : public GLbehavior
	{
	private:
		typedef BehaviorBullSpawner this_type;
	public:
		BehaviorBullSpawner(std::function<void(this_type&)> _initConfig = [](this_type&) {}) : GLbehavior() { _initConfig(*this); }

		void start() {
			mSpawnTimer = 0.0f;
		}

		void update(float _deltaTime) {
			if ((mSpawnTimer += _deltaTime) > mSpawnTime) {
				mSpawnTimer = 0.0f;
				GenPrefabBull(ConfigPrefabBull1);
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