#ifndef CCKIT_PREFAB_BOX_H
#define CCKIT_PREFAB_BOX_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Model Primitives/ModelBox.h"

namespace cckit
{
	
	GLobj& GenPrefabBox(std::function<void()> _config) {
		GLobj& box = *GLfactory<GLobj>::generate();
		box.load_model(modelBoxConfig);
		box.set_shader(*GLfactory<GLshader>::generate());
		box.shader().load("Shaders/shader1.vs", "Shaders/shader1.fs");
		auto pairGConfig2LConfig
			= GLshader::mMapShaderPath2FsGLConfig
				[GLshader::mStringHash(box.shader().vs_path())]
				[GLshader::mStringHash(box.shader().fs_path())];
		box.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		box.shader().mFsLocalConfig = pairGConfig2LConfig.second;

		return box;
	}

	std::function<void()> ConfigPrefabBox0
		= []() {
		
	};
}

#endif // !CCKIT_PREFAB_BOX_H