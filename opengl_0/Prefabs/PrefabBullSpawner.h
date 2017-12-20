#ifndef CCKIT_PREFAB_BULL_SPAWNER_H
#define CCKIT_PREFAB_BULL_SPAWNER_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Behaviors/BehaviorBullSpawner.h"

namespace cckit
{
	GLobj& GenPrefabBullSpawner(std::function<void(BehaviorBullSpawner&)> _config) {
		GLobj& spawner = *GLfactory<GLobj>::generate();
		spawner.add_behavior(new BehaviorBullSpawner(_config));

		return spawner;
	}

	std::function<void(BehaviorBullSpawner&)> ConfigPrefabBullSpawner0
		= [](BehaviorBullSpawner& _behavior) {
		_behavior.mSpawnTime = 3.0f;
	};
}

#endif // !CCKIT_PREFAB_BULL_SPAWNER_H