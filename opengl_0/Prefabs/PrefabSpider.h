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
		spider.shader().load("Shaders/shader0.vs", "Shaders/shader0.fs");
		spider.shader().mFsConfig
			= GLshader::mMapShaderPath2FsConfig
			[GLshader::mStringHash(spider.shader().vs_path())]
			[GLshader::mStringHash(spider.shader().fs_path())];

		return spider;
	}

	std::function<void(BehaviorSpider&)> ConfigPrefabSpider0
		= [](BehaviorSpider& _behavior) {
		_behavior.mSpecularColor = glm::vec3(1);
		_behavior.mShininess = 32;
	};
}

#endif // !CCKIT_PREFAB_SPIDER_H