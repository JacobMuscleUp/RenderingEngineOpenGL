#ifndef CCKIT_PREFAB_BULL_H
#define CCKIT_PREFAB_BULL_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Behaviors/behaviors.h"

namespace cckit
{
	GLobj& GenPrefabBull(std::function<void(BehaviorBull&)> _config) {
		GLobj& bull = *GLfactory<GLobj>::generate();
		bull.load_model("Resources/OBJ/bull/bull.obj");
		bull.set_shader(*pShaderDiffuse);
		bull.add_behavior(new BehaviorBull(_config));
		
		return bull;
	}

	std::function<void(BehaviorBull&)> ConfigPrefabBull0
		= [](BehaviorBull& _behavior) {
		_behavior.mMoveSpeed = 0.22f;
	};
}

#endif // !CCKIT_PREFAB_BULL_H