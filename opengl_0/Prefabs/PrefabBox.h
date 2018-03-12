#ifndef CCKIT_PREFAB_BOX_H
#define CCKIT_PREFAB_BOX_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Model Primitives/ModelBox.h"

namespace cckit
{
	
	GLobj& GenPrefabBox(std::function<void(GLobj&)> _config) {
		GLobj& box = *GLfactory<GLobj>::generate();
		box.load_model(modelBoxConfig);
		box.set_shader(*GLfactory<GLshader>::generate());
		box.shader().load("Shaders/ShaderDiffuse.vs", "Shaders/ShaderDiffuse.fs");
		auto pairGConfig2LConfig
			= GLshader::mMapShaderPath2FsGLConfig
				[GLshader::mStringHash(box.shader().vs_path())]
				[GLshader::mStringHash(box.shader().fs_path())];
		box.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		box.shader().mFsLocalConfig = pairGConfig2LConfig.second;

		_config(box);

		return box;
	}

	std::function<void(GLobj&)> ConfigPrefabBox0
		= [](GLobj& _obj) {
		_obj.set_position(glm::vec3(0, 0, -3));
		_obj.mScale = glm::vec3(0.1);
		cckit::GLrenderer& rend = *_obj.renderer_ptr();
		rend.mDiffuseColor = glm::vec3(1, 0, 1);
		rend.mSpecularColor = glm::vec3(1);
		rend.mShininess = 32;
		_obj.apply_renderer_config();// necessary since renderer configs are performed outside GLbehavior::start()
	};
}

#endif // !CCKIT_PREFAB_BOX_H