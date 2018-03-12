#ifndef CCKIT_PREFAB_SPIDER_H
#define CCKIT_PREFAB_SPIDER_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Behaviors/BehaviorSpider.h"

namespace cckit
{
	GLobj& GenPrefabSpider(std::function<void(BehaviorSpider&)> _config) {
		GLobj& spider = *GLfactory<GLobj>::generate();
		spider.load_model("Resources/OBJ/spider/spider.obj");
		spider.add_behavior(new BehaviorSpider(_config));
		spider.set_shader(*GLfactory<GLshader>::generate());
		spider.shader().load("Shaders/ShaderTexture.vs", "Shaders/ShaderTexture.fs");
		auto pairGConfig2LConfig
			= GLshader::mMapShaderPath2FsGLConfig
				[GLshader::mStringHash(spider.shader().vs_path())]
				[GLshader::mStringHash(spider.shader().fs_path())];
		spider.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		spider.shader().mFsLocalConfig = pairGConfig2LConfig.second;

		return spider;
	}

	std::function<void(BehaviorSpider&)> ConfigPrefabSpider0
		= [](BehaviorSpider& _behavior) {
		_behavior.mSpecularColor = glm::vec3(1);
		_behavior.mShininess = 32;
	};
}

#endif // !CCKIT_PREFAB_SPIDER_H