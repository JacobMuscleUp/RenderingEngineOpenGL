#ifndef CCKIT_PREFAB_GROUND_H
#define CCKIT_PREFAB_GROUND_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Model Primitives/ModelGround.h"

namespace cckit
{
	GLobj& GenPrefabGround(std::function<void(BehaviorGround&)> _config) {
		GLobj& ground = *GLfactory<GLobj>::generate();
		ground.load_model(modelGroundConfig);
		ground.add_behavior(new BehaviorGround(_config));
		ground.set_shader(*GLfactory<GLshader>::generate());
		ground.shader().load("Shaders/shaderGround.vs", "Shaders/shaderGround.fs");
		auto pairGConfig2LConfig
			= GLshader::mMapShaderPath2FsGLConfig
			[GLshader::mStringHash(ground.shader().vs_path())]
		[GLshader::mStringHash(ground.shader().fs_path())];
		ground.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		ground.shader().mFsLocalConfig = pairGConfig2LConfig.second;

		return ground;
	}

	std::function<void(BehaviorGround&)> ConfigPrefabGround0
		= [](BehaviorGround& _behavior) {
	};
}

#endif // !CCKIT_PREFAB_GROUND_H