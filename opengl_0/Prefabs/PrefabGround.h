#ifndef CCKIT_PREFAB_GROUND_H
#define CCKIT_PREFAB_GROUND_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Model Primitives/ModelGround.h"

namespace cckit
{
	/*GLobj& GenPrefabGround(std::function<void(BehaviorGround&)> _config) {
		GLobj& ground = *GLfactory<GLobj>::generate();
		ground.load_model(modelGroundConfig);
		ground.add_behavior(new BehaviorGround(_config));
		ground.set_shader(*GLfactory<GLshader>::generate());
		//ground.shader().load("Shaders/ShaderDiffuse.vs", "Shaders/ShaderDiffuse.fs");
		ground.shader().load("Shaders/ShaderTexturePom.vs", "Shaders/ShaderTexturePom.fs");
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
	};*/

	GLobj& GenPrefabGround(std::function<void(GLobj&)> _config) {
		GLobj& ground = *GLfactory<GLobj>::generate();
		ground.load_model(modelGroundConfig);
		//ground.add_behavior(new BehaviorGround([](BehaviorGround& _behavior) {}));
		ground.set_shader(*GLfactory<GLshader>::generate());
		//ground.shader().load("Shaders/ShaderDiffuse.vs", "Shaders/ShaderDiffuse.fs");
		ground.shader().load("Shaders/ShaderTexturePom.vs", "Shaders/ShaderTexturePom.fs");
		auto pairGConfig2LConfig
			= GLshader::mMapShaderPath2FsGLConfig
			[GLshader::mStringHash(ground.shader().vs_path())]
		[GLshader::mStringHash(ground.shader().fs_path())];
		ground.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		ground.shader().mFsLocalConfig = pairGConfig2LConfig.second;

		_config(ground);

		return ground;
	}

	std::function<void(GLobj&)> ConfigPrefabGround0
		= [](GLobj& _obj) {
		_obj.set_position(glm::vec3(0, -2, 0));
		_obj.mScale = glm::vec3(4);
		cckit::GLrenderer& rend = *_obj.renderer_ptr();
		rend.mDiffuseColor = glm::vec3(1, 0, 0);
		rend.mSpecularColor = glm::vec3(1);
		rend.mShininess = 32;
		_obj.apply_renderer_config();// necessary since renderer configs are performed outside GLbehavior::start()
	};
}

#endif // !CCKIT_PREFAB_GROUND_H