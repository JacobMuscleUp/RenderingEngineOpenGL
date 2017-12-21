#ifndef CCKIT_PREFAB_BULL_H
#define CCKIT_PREFAB_BULL_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Behaviors/BehaviorBull.h"

namespace cckit
{
	GLobj& GenPrefabBull(std::function<void(BehaviorBull&)> _config) {
		GLobj& bull = *GLfactory<GLobj>::generate();
		bull.load_model("Resources/OBJ/bull/bull.obj");
		bull.add_behavior(new BehaviorBull(_config));
		bull.set_shader(*GLfactory<GLshader>::generate());
		bull.shader().load("Shaders/shader1.vs", "Shaders/shader1.fs");
		auto pairGConfig2LConfig 
			= GLshader::mMapShaderPath2FsGLConfig
				[GLshader::mStringHash(bull.shader().vs_path())]
				[GLshader::mStringHash(bull.shader().fs_path())];
		bull.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		bull.shader().mFsLocalConfig = pairGConfig2LConfig.second;
				
		return bull;
	}

	std::function<void(BehaviorBull&)> ConfigPrefabBull0
		= [](BehaviorBull& _behavior) {
		_behavior.mMoveSpeed = 0.22f;

		_behavior.mDiffuseColor = glm::vec3(1, 0, 0);
		_behavior.mSpecularColor = glm::vec3(1);
		_behavior.mShininess = 32;
	};

	std::function<void(BehaviorBull&)> ConfigPrefabBull1
		= [](BehaviorBull& _behavior) {
		_behavior.mMoveSpeed = 0.22f;

		_behavior.mDiffuseColor = glm::vec3(0, 0, 1);
		_behavior.mSpecularColor = glm::vec3(1);
		_behavior.mShininess = 32;
	};
}

#endif // !CCKIT_PREFAB_BULL_H