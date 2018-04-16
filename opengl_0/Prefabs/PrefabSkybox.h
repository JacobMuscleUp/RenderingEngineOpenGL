#ifndef CCKIT_PREFAB_SKYBOX_H
#define CCKIT_PREFAB_SKYBOX_H

#include "../RenderingEngineOpenGL/GLobj.h"
#include "../Model Primitives/ModelSkybox.h"

namespace cckit
{

	GLobj& GenPrefabSkybox(std::function<void(GLobj&)> _config) {
		GLobj& skybox = *GLfactory<GLobj>::generate();
		
		skybox.load_model(modelSkyboxConfig);
		skybox.set_shader(*GLfactory<GLshader>::generate());
		skybox.shader().load("Shaders/ShaderDiffuse.vs", "Shaders/ShaderDiffuse.fs");
		/*skybox.shader().load("Shaders/shaderSkybox.vs", "Shaders/shaderSkybox.fs");
		std::vector<std::string> faces;
		faces.reserve(6);
		faces.push_back("../Resources/SKYBOX/right.jpg");
		faces.push_back("../Resources/SKYBOX/left.jpg");
		faces.push_back("../Resources/SKYBOX/top.jpg");
		faces.push_back("../Resources/SKYBOX/bottom.jpg");
		faces.push_back("../Resources/SKYBOX/back.jpg");
		faces.push_back("../Resources/SKYBOX/front.jpg");
		GLuint textureHandle = glLoadCubeMap(faces);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);*/
		
		auto pairGConfig2LConfig
			= GLshader::mMapShaderPath2FsGLConfig
			[GLshader::mStringHash(skybox.shader().vs_path())]
		[GLshader::mStringHash(skybox.shader().fs_path())];
		skybox.shader().mFsGlobalConfig = pairGConfig2LConfig.first;
		skybox.shader().mFsLocalConfig = pairGConfig2LConfig.second;

		_config(skybox);

		return skybox;
	}

	std::function<void(GLobj&)> ConfigPrefabSkybox0
		= [](GLobj& _obj) {
		_obj.set_position(glm::vec3(0, 0, 0));
		_obj.mScale = glm::vec3(7);
		cckit::GLrenderer& rend = *_obj.renderer_ptr();
		rend.mDiffuseColor = glm::vec3(0, 1, 1);
		rend.mSpecularColor = glm::vec3(1);
		rend.mShininess = 32;
		_obj.apply_renderer_config();// necessary since renderer configs are performed outside GLbehavior::start()
	};

	std::function<void(GLobj&)> ConfigPrefabSkybox1
		= [](GLobj& _obj) {
		_obj.set_position(glm::vec3(0, -3, 0));
		_obj.mScale = glm::vec3(2);
		cckit::GLrenderer& rend = *_obj.renderer_ptr();
		rend.mDiffuseColor = glm::vec3(0, 1, 1);
		rend.mSpecularColor = glm::vec3(1);
		rend.mShininess = 32;
		_obj.apply_renderer_config();// necessary since renderer configs are performed outside GLbehavior::start()
	};
}

#endif // !CCKIT_PREFAB_SKYBOX_H