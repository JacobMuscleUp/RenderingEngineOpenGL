#ifndef CCKIT_PREFAB_LAMP_H
#define CCKIT_PREFAB_LAMP_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Behaviors/BehaviorLamp.h"
#include "../global.h"

namespace cckit
{
	GLobj& GenPrefabLamp(std::function<void(BehaviorLamp&)> _config) {
		GLobj& lamp = *GLfactory<GLobj>::generate();
		lamp.load_model("Resources/OBJ/box/box.obj");
		lamp.add_behavior(new BehaviorLamp(_config));
		lamp.set_shader(*GLfactory<GLshader>::generate());
		lamp.shader().load("Shaders/shaderLamp.vs", "Shaders/shaderLamp.fs");
		auto pairGConfig2LConfig
			= GLshader::mMapShaderPath2FsGLConfig
				[GLshader::mStringHash(lamp.shader().vs_path())]
				[GLshader::mStringHash(lamp.shader().fs_path())];
		lamp.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		lamp.shader().mFsLocalConfig = pairGConfig2LConfig.second;

		return lamp;
	}

	std::function<void(BehaviorLamp&)> ConfigPrefabLamp0
		= [](BehaviorLamp& _behavior) {
		_behavior.mStartingPos = ptLights[0].mPos;
	};
}

#endif // !CCKIT_PREFAB_LAMP_H