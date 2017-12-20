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
		lamp.shader().mFsConfig
			= GLshader::mMapShaderPath2FsConfig
			[GLshader::mStringHash(lamp.shader().vs_path())]
			[GLshader::mStringHash(lamp.shader().fs_path())];

		return lamp;
	}

	std::function<void(BehaviorLamp&)> ConfigPrefabLamp0
		= [](BehaviorLamp& _behavior) {
		_behavior.mStartingPos = ptLights[0].mPos;
	};
}

#endif // !CCKIT_PREFAB_LAMP_H