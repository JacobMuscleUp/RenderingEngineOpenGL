#include "cckitdef.h"

#include "RenderingEngineOpenGL/GLconfig.h"
#include "RenderingEngineOpenGL/GLshader.h"
#include "RenderingEngineOpenGL/GLmodel.h"
#include "RenderingEngineOpenGL/GLcamera.h"
#include "RenderingEngineOpenGL/GLobj.h"
#include "RenderingEngineOpenGL/GLutils.h"
#include "RenderingEngineOpenGL/GLfactory.h"

#include "Behaviors/behaviors.h"
#include "Prefabs/prefabs.h"
#include "global.h"
#include "afx_config.h"

using std::cout; using std::endl; using std::cin;

void fps_assimp(GLFWwindow* _pWindow);
void process_keyboard(GLFWwindow* _pWindow, cckit::GLcamera& _camera, float _deltaTime);
void setup_fsConfigs();

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
	int temp;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* pWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "test0", nullptr, nullptr);
	if (!pWindow) {
		cout << "window creation failed" << endl;
		glfwTerminate();
		cin >> temp;
		return -1;
	}
	glfwMakeContextCurrent(pWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "glad init failed" << endl;
		glfwTerminate();
		cin >> temp;
		return -1;
	}

	GLFWframebuffersizefun framebufferSizeCallback = [](GLFWwindow* _pWindow, int _width, int _height) {
		glViewport(0, 0, _width, _height);
		cout << "framebuffer resized" << endl;
	};
	glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
	glfwSetWindowCloseCallback(pWindow, [](GLFWwindow*) {
		cout << "window closed" << endl;
	});

	fps_assimp(pWindow);

	glfwTerminate();
	cin >> temp;
	return 0;
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
cckit::BehaviorLamp lampBehavior;
float heightScale = 0.1f;

void fps_assimp2(GLFWwindow* _pWindow)
{
	glfwSetInputMode(_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(_pWindow, [](GLFWwindow* pWindow, double posX, double posY) {
		static bool bFirstEntry = true;
		static float lastPosX, lastPosY;
		if (bFirstEntry) {
			bFirstEntry = false;
			lastPosX = posX;
			lastPosY = posY;
		}

		float offsetX = posX - lastPosX
			, offsetY = lastPosY - posY;
		lastPosX = posX;
		lastPosY = posY;

		camera.process_mouse(offsetX, offsetY);
	});

	pShaderCoordAxes = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderOutline = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderScreen = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderDepthMap = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderDepthDebug = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderCoordAxes->load("Shaders/shaderMonoColor0.vs", "Shaders/shaderMonoColor0.fs");
	pShaderOutline->load("Shaders/shaderMonoColor1.vs", "Shaders/shaderMonoColor1.fs");
	pShaderScreen->load("Shaders/ShaderFramebuffer.vs", "Shaders/ShaderFramebuffer.fs");
	pShaderDepthMap->load("Shaders/ShaderDepthMap.vs", "Shaders/ShaderDepthMap.fs");
	pShaderDepthDebug->load("Shaders/ShaderDepthDebug.vs", "Shaders/ShaderDepthDebug.fs");
	setup_fsConfigs();

	cckit::GLfactory<cckit::GLobj> objFactory;
	cckit::GLobj& spider = cckit::GenPrefabSpider(cckit::ConfigPrefabSpider0);
	cckit::GLobj& bull = cckit::GenPrefabBull(cckit::ConfigPrefabBull0);
	cckit::GLobj& lamp = cckit::GenPrefabLamp(cckit::ConfigPrefabLamp0);
	cckit::GLobj& spawner = cckit::GenPrefabBullSpawner(cckit::ConfigPrefabBullSpawner0);
	cckit::GLobj& box = cckit::GenPrefabBox(cckit::ConfigPrefabBox0);
	cckit::GLobj& skybox = cckit::GenPrefabSkybox(cckit::ConfigPrefabSkybox0);
	cckit::GLobj& ground = cckit::GenPrefabGround(cckit::ConfigPrefabGround0);
	lampBehavior = *lamp.get_behavior<cckit::BehaviorLamp>();

#ifdef POSTPROCESS_ENABLED
	cckit::GLframebuffer fbo(SCREEN_WIDTH, SCREEN_HEIGHT);
	cckit::GLquad quad;
	pShaderScreen->set1i("screenTexture", 0);// attach "screenTexture" to GL_TEXTURE0 where screenTexture in sampler2D
#endif

	float deltaTime
		, lastFrameTime = glfwGetTime();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while (!glfwWindowShouldClose(_pWindow)) {// loop
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		process_keyboard(_pWindow, camera, deltaTime);

#ifdef POSTPROCESS_ENABLED
		fbo.set_active();
#endif

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cckit::GLobj::globally_start_behaviors();
		cckit::GLobj::globally_update_behaviors(deltaTime);
		cckit::GLobj::globally_render(&camera
			, [](cckit::GLcamera& _camera) {
			_camera.set_perspective<cckit::GLcamera::projection::viewSpace>(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);
			_camera.set_shader_outline(*pShaderOutline);
			_camera.set_shader_coord_axes(*pShaderCoordAxes);
		}
			, [&](cckit::GLcamera& _camera, const cckit::GLobj& _obj) {
			_camera.render(_obj);
		});

		glDisable(GL_DEPTH_TEST);

#ifdef POSTPROCESS_ENABLED
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		pShaderScreen->use();
		quad.prepare();
		fbo.push_texture(GL_TEXTURE0);
		quad.render(GL_FILL);
#endif

		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
	}//! loop

	cckit::GLmodel::unload();
	cckit::GLshader::unload();
}

void fps_assimp(GLFWwindow* _pWindow)
{
	glfwSetInputMode(_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(_pWindow, [](GLFWwindow* pWindow, double posX, double posY) {
		static bool bFirstEntry = true;
		static float lastPosX, lastPosY;
		if (bFirstEntry) {
			bFirstEntry = false;
			lastPosX = posX;
			lastPosY = posY;
		}

		float offsetX = posX - lastPosX
			, offsetY = lastPosY - posY;
		lastPosX = posX;
		lastPosY = posY;

		camera.process_mouse(offsetX, offsetY);
	});

	dirLightDir = glm::vec3(0.0f, -1.0f, 1.0f);
	lightSpaceMatrix = glm::lookAtRH(ptLights[0].mPos + glm::vec3(0.0, 3.0, 0.0), ptLights[0].mPos + glm::vec3(0.0, 3.0, 0.0) + dirLightDir);

	pShaderCoordAxes = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderOutline = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderScreen = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderDepthMap = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderDepthDebug = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderCoordAxes->load("Shaders/shaderMonoColor0.vs", "Shaders/shaderMonoColor0.fs");
	pShaderOutline->load("Shaders/shaderMonoColor1.vs", "Shaders/shaderMonoColor1.fs");
	pShaderScreen->load("Shaders/ShaderFramebuffer.vs", "Shaders/ShaderFramebuffer.fs");
	pShaderDepthMap->load("Shaders/ShaderDepthMap.vs", "Shaders/ShaderDepthMap.fs");
	pShaderDepthDebug->load("Shaders/ShaderDepthDebug.vs", "Shaders/ShaderDepthDebug.fs");
	setup_fsConfigs();

	cckit::GLfactory<cckit::GLobj> objFactory;
	cckit::GLobj& spider = cckit::GenPrefabSpider(cckit::ConfigPrefabSpider0);
	cckit::GLobj& bull = cckit::GenPrefabBull(cckit::ConfigPrefabBull0);
	cckit::GLobj& lamp = cckit::GenPrefabLamp(cckit::ConfigPrefabLamp0);
	cckit::GLobj& spawner = cckit::GenPrefabBullSpawner(cckit::ConfigPrefabBullSpawner0);
	cckit::GLobj& box = cckit::GenPrefabBox(cckit::ConfigPrefabBox0);
	cckit::GLobj& skybox = cckit::GenPrefabSkybox(cckit::ConfigPrefabSkybox0);
	cckit::GLobj& ground = cckit::GenPrefabGround(cckit::ConfigPrefabGround0);
	lampBehavior = *lamp.get_behavior<cckit::BehaviorLamp>();

	cckit::GLframebufferStack fboStack;

#ifdef POSTPROCESS_ENABLED
	cckit::GLframebuffer fbo(SCREEN_WIDTH, SCREEN_HEIGHT);
	cckit::GLquad quadFbo;
	pShaderScreen->set1i("screenTexture", 0);// attach "screenTexture" to GL_TEXTURE0 where screenTexture in sampler2D
#endif

#ifdef SHADOW_MAPPING_ENABLED
	cckit::GLframebufferDepthMap fboDepthMap(SCREEN_WIDTH, SCREEN_HEIGHT);
	cckit::GLquad quadFboDepthMap;
	pShaderDepthDebug->set1i("depthMap", 0);

	
#endif

	float deltaTime
		, lastFrameTime = glfwGetTime();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(_pWindow)) {// loop
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		process_keyboard(_pWindow, camera, deltaTime);

#ifdef POSTPROCESS_ENABLED
		fboStack.push(fbo);
#endif

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef SHADOW_MAPPING_ENABLED
		fboStack.push(fboDepthMap);
		glClear(GL_DEPTH_BUFFER_BIT);
#endif

		cckit::GLobj::globally_start_behaviors();
		cckit::GLobj::globally_update_behaviors(deltaTime);
		cckit::GLobj::globally_render(&camera
			, [](cckit::GLcamera& _camera) {
			_camera.set_ortho<cckit::GLcamera::projection::viewSpace>(-1.0f, 1.0f,  (float)-SCREEN_WIDTH / SCREEN_HEIGHT, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
			//_camera.set_perspective<cckit::GLcamera::projection::viewSpace>(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 1000.0f);
			_camera.set_shader_outline(*pShaderOutline);
			_camera.set_shader_coord_axes(*pShaderCoordAxes);
		}
			, [&](cckit::GLcamera& _camera, const cckit::GLobj& _obj) {
#ifndef SHADOW_MAPPING_ENABLED
			_camera.render(_obj);
			//_camera.render(_obj, lightSpaceMatrix);
#else
			//_camera.render(_obj);
			_camera.render2depthMap(_obj, lightSpaceMatrix, *pShaderDepthMap);
			//_camera.render2depthMap(_obj, _camera.get_view_matrix(), *pShaderDepthMap);
#endif
		});

#ifdef SHADOW_MAPPING_ENABLED
		fboStack.pop();
		/*pShaderDepthDebug->use();
		quadFboDepthMap.prepare();
		fboDepthMap.push_texture(GL_TEXTURE0); 
		quadFboDepthMap.render(GL_FILL);*/

		cckit::GLobj::globally_start_behaviors();
		cckit::GLobj::globally_update_behaviors(deltaTime);
		cckit::GLobj::globally_render(&camera
			, [](cckit::GLcamera& _camera) {
			_camera.set_perspective<cckit::GLcamera::projection::viewSpace>(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);
			_camera.set_ortho<cckit::GLcamera::projection::lightSpace>(-1.0f, 1.0f, (float)-SCREEN_WIDTH / SCREEN_HEIGHT, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
			_camera.set_shader_outline(*pShaderOutline);
			_camera.set_shader_coord_axes(*pShaderCoordAxes);
		}
			, [&](cckit::GLcamera& _camera, const cckit::GLobj& _obj) {
			fboDepthMap.push_texture(GL_TEXTURE0);
			_camera.render(_obj);
		});
#endif

		glDisable(GL_DEPTH_TEST);

#ifdef POSTPROCESS_ENABLED
		fboStack.pop();
		glClear(GL_COLOR_BUFFER_BIT);
		pShaderScreen->use();
		quadFbo.prepare();
		fbo.push_texture(GL_TEXTURE0);
		quadFbo.render(GL_FILL);
#endif

		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
	}//! loop

	cckit::GLmodel::unload();
	cckit::GLshader::unload();
}

void process_keyboard(GLFWwindow* _pWindow, cckit::GLcamera& _camera, float _deltaTime)
{
	if (glfwGetKey(_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwGetKey(_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS
		|| glfwGetKey(_pWindow, GLFW_KEY_ENTER) == GLFW_PRESS)
		glfwSetWindowShouldClose(_pWindow, true);

	if (glfwGetKey(_pWindow, GLFW_KEY_W) == GLFW_PRESS)
		_camera.process_keyboard(cckit::GLcamera::movement::forward, _deltaTime);
	else if (glfwGetKey(_pWindow, GLFW_KEY_S) == GLFW_PRESS)
		_camera.process_keyboard(cckit::GLcamera::movement::backward, _deltaTime);
	if (glfwGetKey(_pWindow, GLFW_KEY_A) == GLFW_PRESS)
		_camera.process_keyboard(cckit::GLcamera::movement::left, _deltaTime);
	else if (glfwGetKey(_pWindow, GLFW_KEY_D) == GLFW_PRESS)
		_camera.process_keyboard(cckit::GLcamera::movement::right, _deltaTime);
    if (glfwGetKey(_pWindow, GLFW_KEY_UP) == GLFW_PRESS)
		lampBehavior.obj().set_position(lampBehavior.obj().position() + glm::vec3(0, 0, _deltaTime));
	else if (glfwGetKey(_pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
		lampBehavior.obj().set_position(lampBehavior.obj().position() - glm::vec3(0, 0, _deltaTime));
	if (glfwGetKey(_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lampBehavior.obj().set_position(lampBehavior.obj().position() + glm::vec3(_deltaTime, 0, 0));
	else if (glfwGetKey(_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
		lampBehavior.obj().set_position(lampBehavior.obj().position() - glm::vec3(_deltaTime, 0, 0));
	else if (glfwGetKey(_pWindow, GLFW_KEY_O) == GLFW_PRESS) {
		dirLightDir = glm::vec3(dirLightDir.x, dirLightDir.y, dirLightDir.z + _deltaTime);
		lightSpaceMatrix = glm::lookAtRH(ptLights[0].mPos + glm::vec3(0.0, 3.0, 0.0), ptLights[0].mPos + glm::vec3(0.0, 3.0, 0.0) + dirLightDir);
	}
	else if (glfwGetKey(_pWindow, GLFW_KEY_P) == GLFW_PRESS) {
		dirLightDir = glm::vec3(dirLightDir.x, dirLightDir.y, dirLightDir.z - _deltaTime);
		lightSpaceMatrix = glm::lookAtRH(ptLights[0].mPos + glm::vec3(0.0, 3.0, 0.0), ptLights[0].mPos + glm::vec3(0.0, 3.0, 0.0) + dirLightDir);
	}

	if (glfwGetKey(_pWindow, GLFW_KEY_N) == GLFW_PRESS)
		heightScale -= _deltaTime;
	else if (glfwGetKey(_pWindow, GLFW_KEY_M) == GLFW_PRESS)
		heightScale += _deltaTime;
}

void setup_fsConfigs() {
	auto fsGlobalConfig
		= [](const cckit::GLshader& _shader) {
		glm::vec3 dirLightDiffuse = ptLights[0].mColor * glm::vec3(0.7f)
			, dirLightAmbient = dirLightDiffuse * glm::vec3(0.2f)
			, dirLightSpecular = glm::vec3(1.0f);
		glm::vec3 ptLightDiffuse = ptLights[0].mColor * glm::vec3(0.7f)
			, ptLightAmbient = dirLightDiffuse * glm::vec3(0.2f)
			, ptLightSpecular = glm::vec3(1.0f);
		glm::vec3 spotLightDiffuse = glm::vec3(0, 1, 0)
			, spotLightAmbient = dirLightDiffuse * glm::vec3(0.2f)
			, spotLightSpecular = glm::vec3(1.0f);

		_shader.set3fv("dirLight.dir", dirLightDir);
		_shader.set3fv("dirLight.ambient", dirLightAmbient);
		_shader.set3fv("dirLight.diffuse", dirLightDiffuse);
		_shader.set3fv("dirLight.specular", dirLightSpecular);

		_shader.set3fv("ptLight.pos", lampBehavior.obj().position());
		_shader.set3fv("ptLight.ambient", ptLightAmbient);
		_shader.set3fv("ptLight.diffuse", ptLightDiffuse);
		_shader.set3fv("ptLight.specular", ptLightSpecular);
		_shader.set1f("ptLight.attenConstant", 1.0f);
		_shader.set1f("ptLight.attenLinear", 0.09f);
		_shader.set1f("ptLight.attenQuadratic", 0.032f);

		_shader.set3fv("spotLight.pos", camera.pos());
		_shader.set3fv("spotLight.dir", camera.forward());
		_shader.set1f("spotLight.innerCutoffCosine", glm::cos(glm::radians(8.0f)));
		_shader.set1f("spotLight.outerCutoffCosine", glm::cos(glm::radians(11.0f)));
		_shader.set3fv("spotLight.ambient", spotLightAmbient);
		_shader.set3fv("spotLight.diffuse", spotLightDiffuse);
		_shader.set3fv("spotLight.specular", spotLightSpecular);
		_shader.set1f("spotLight.attenConstant", 1.0f);
		_shader.set1f("spotLight.attenLinear", 0.09f);
		_shader.set1f("spotLight.attenQuadratic", 0.032f);

		_shader.set3fv("viewPos", camera.pos());

		_shader.setmatrix4fv("matLightSpace", 1, GL_FALSE, lightSpaceMatrix);
		_shader.set1i("depthMap", 0);
	};
	auto fsGlobalConfigTextureNM
		= [&fsGlobalConfig](const cckit::GLshader& _shader) {
		fsGlobalConfig(_shader);
		_shader.set1f("heightScale", heightScale);
	};

	auto fsLocalConfigTexture
		= [](const cckit::GLshader& _shader, const cckit::GLrenderer& _renderer) {
		_shader.set3fv("material1.specular", _renderer.mSpecularColor);
		_shader.set1i("material1.shininess", _renderer.mShininess);
	};
	auto fsLocalConfigDiffuse
		= [](const cckit::GLshader& _shader, const cckit::GLrenderer& _renderer) {
		_shader.set3fv("material.diffuse", _renderer.mDiffuseColor);
		_shader.set3fv("material.specular", _renderer.mSpecularColor);
		_shader.set1i("material.shininess", _renderer.mShininess);
	};
	auto fsLocalConfigTextureNM
		= [&fsLocalConfigTexture](const cckit::GLshader& _shader, const cckit::GLrenderer& _renderer) {
		fsLocalConfigTexture(_shader, _renderer);
	};

	cckit::GLshader::mMapShaderPath2FsGLConfig
		[cckit::GLshader::mStringHash("Shaders/ShaderTexture.vs")]
		[cckit::GLshader::mStringHash("Shaders/ShaderTexture.fs")]
	= std::pair<std::function<void(const cckit::GLshader&)>, std::function<void(const cckit::GLshader&, const cckit::GLrenderer&)> >
		(fsGlobalConfig, fsLocalConfigTexture);
	cckit::GLshader::mMapShaderPath2FsGLConfig
		[cckit::GLshader::mStringHash("Shaders/ShaderDiffuse.vs")]
		[cckit::GLshader::mStringHash("Shaders/ShaderDiffuse.fs")]
	= std::pair<std::function<void(const cckit::GLshader&)>, std::function<void(const cckit::GLshader&, const cckit::GLrenderer&)> >
		(fsGlobalConfig, fsLocalConfigDiffuse);
	cckit::GLshader::mMapShaderPath2FsGLConfig
		[cckit::GLshader::mStringHash("Shaders/ShaderTexturePom.vs")]
		[cckit::GLshader::mStringHash("Shaders/ShaderTexturePom.fs")]
	= std::pair<std::function<void(const cckit::GLshader&)>, std::function<void(const cckit::GLshader&, const cckit::GLrenderer&)> >
		(fsGlobalConfigTextureNM, fsLocalConfigTextureNM);
		
	cckit::GLshader::mMapShaderPath2FsGLConfig
		[cckit::GLshader::mStringHash("Shaders/shaderLamp.vs")]
		[cckit::GLshader::mStringHash("Shaders/shaderLamp.fs")]
	= std::pair<std::function<void(const cckit::GLshader&)>, std::function<void(const cckit::GLshader&, const cckit::GLrenderer&)> >
		([](const cckit::GLshader&) {}, [](const cckit::GLshader&, const cckit::GLrenderer&) {});

	cckit::GLshader::mMapShaderPath2FsGLConfig
		[cckit::GLshader::mStringHash("Shaders/shaderSkybox.vs")]
	[cckit::GLshader::mStringHash("Shaders/shaderSkybox.fs")]
	= std::pair<std::function<void(const cckit::GLshader&)>, std::function<void(const cckit::GLshader&, const cckit::GLrenderer&)> >
		([](const cckit::GLshader&) {}, [](const cckit::GLshader&, const cckit::GLrenderer&) {});
}

////////////////////////////////
//////// REVISION
/////////////////////////////////
int main2() {
	int temp;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* pWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "test0", nullptr, nullptr);
	if (!pWindow) {
		cout << "window creation failed" << endl;
		glfwTerminate();
		cin >> temp;
		return -1;
	}
	glfwMakeContextCurrent(pWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "glad init failed" << endl;
		glfwTerminate();
		cin >> temp;
		return -1;
	}

	GLFWframebuffersizefun framebufferSizeCallback = [](GLFWwindow* _pWindow, int _width, int _height) {
		glViewport(0, 0, _width, _height);
		cout << "framebuffer resized" << endl;
	};
	glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
	glfwSetWindowCloseCallback(pWindow, [](GLFWwindow*) {
		cout << "window closed" << endl;
	});

	GLfloat triVertices[] = {
		-0.5, -1.0, 0.0,  0.0, 0.0
		, 1.0, 0.0, 0.0,  1.0, 0.0
		, 0.5, 1.0, 0.0,  0.5, 1.0
	};
	GLuint vaoHandle, vboHandle;
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &vboHandle);
	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	
	cckit::GLfactory<cckit::GLshader> factory;
	cckit::GLshader& myShader = *cckit::GLfactory<cckit::GLshader>::generate();
	myShader.load("s.vs", "s.fs");

	GLuint textureHandle;
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	int width, height, numComponents;
	unsigned char* data = stbi_load("Resources/bricks2.jpg", &width, &height, &numComponents, 0);
	if (data) {
		GLenum format;
		if (numComponents == 1)
			format = GL_RED;
		else if (numComponents == 3)
			format = GL_RGB;
		else if (numComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Texture failed to load at path: " << std::endl;
	}
	stbi_image_free(data);

	//
	myShader.use();
	myShader.set1i("textureMap", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	//

	float deltaTime
		, lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(pWindow)) {// loop
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		myShader.use();
		glBindVertexArray(vaoHandle);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	cin >> temp;
	return 0;
}