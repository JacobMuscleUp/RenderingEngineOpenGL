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

	pShaderCoordAxes = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderOutline = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderScreen = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderCoordAxes->load("Shaders/shaderMonoColor0.vs", "Shaders/shaderMonoColor0.fs");
	pShaderOutline->load("Shaders/shaderMonoColor1.vs", "Shaders/shaderMonoColor1.fs");
	pShaderScreen->load("Shaders/ShaderFramebuffer.vs", "Shaders/ShaderFramebuffer.fs");
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
	GLuint fboHandle = fbo.fbo()
		, tboHandle = fbo.texture_color();
	cckit::GLquadVA quadVA;
	GLuint quadVaoHandle = quadVA.vao();
#endif

	pShaderScreen->set1i("screenTexture", 0);// attach "screenTexture" to GL_TEXTURE0 where screenTexture in sampler2D

#ifdef SHADOW_MAPPING_ENABLED
	cckit::GLframebufferDepthMap depthMapFbo(1024, 1024);
#endif

	float deltaTime
		, lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(_pWindow)) {// loop
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		process_keyboard(_pWindow, camera, deltaTime);

#ifdef SHADOW_MAPPING_ENABLED

#endif

#ifdef POSTPROCESS_ENABLED
		glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
#endif

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cckit::GLobj::globally_start_behaviors();
		cckit::GLobj::globally_update_behaviors(deltaTime);
		cckit::GLobj::globally_render(&camera
			, [](cckit::GLcamera& _camera) {
			_camera.set_perspective(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);
			_camera.set_shader_outline(*pShaderOutline);
			_camera.set_shader_coord_axes(*pShaderCoordAxes);
		}
			, [](cckit::GLcamera& _camera, const cckit::GLobj& _obj) {
			_camera.render(_obj);
		});
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

#ifdef POSTPROCESS_ENABLED
		glClear(GL_COLOR_BUFFER_BIT);
		pShaderScreen->use();
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(quadVaoHandle);
		glBindTexture(GL_TEXTURE_2D, tboHandle);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, 6);
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

		_shader.set3fv("dirLight.dir", glm::value_ptr(dirLightDir));
		_shader.set3fv("dirLight.ambient", glm::value_ptr(dirLightAmbient));
		_shader.set3fv("dirLight.diffuse", glm::value_ptr(dirLightDiffuse));
		_shader.set3fv("dirLight.specular", glm::value_ptr(dirLightSpecular));

		_shader.set3fv("ptLight.pos", glm::value_ptr(lampBehavior.obj().position()));
		_shader.set3fv("ptLight.ambient", glm::value_ptr(ptLightAmbient));
		_shader.set3fv("ptLight.diffuse", glm::value_ptr(ptLightDiffuse));
		_shader.set3fv("ptLight.specular", glm::value_ptr(ptLightSpecular));
		_shader.set1f("ptLight.attenConstant", 1.0f);
		_shader.set1f("ptLight.attenLinear", 0.09f);
		_shader.set1f("ptLight.attenQuadratic", 0.032f);

		_shader.set3fv("spotLight.pos", glm::value_ptr(camera.pos()));
		_shader.set3fv("spotLight.dir", glm::value_ptr(camera.forward()));
		_shader.set1f("spotLight.innerCutoffCosine", glm::cos(glm::radians(8.0f)));
		_shader.set1f("spotLight.outerCutoffCosine", glm::cos(glm::radians(11.0f)));
		_shader.set3fv("spotLight.ambient", glm::value_ptr(spotLightAmbient));
		_shader.set3fv("spotLight.diffuse", glm::value_ptr(spotLightDiffuse));
		_shader.set3fv("spotLight.specular", glm::value_ptr(spotLightSpecular));
		_shader.set1f("spotLight.attenConstant", 1.0f);
		_shader.set1f("spotLight.attenLinear", 0.09f);
		_shader.set1f("spotLight.attenQuadratic", 0.032f);

		_shader.set3fv("viewPos", glm::value_ptr(camera.pos()));
	};
	auto fsGlobalConfigTextureNM
		= [&fsGlobalConfig](const cckit::GLshader& _shader) {
		fsGlobalConfig(_shader);
		_shader.set1f("heightScale", heightScale);
	};

	auto fsLocalConfigTexture
		= [](const cckit::GLshader& _shader, const cckit::GLrenderer& _renderer) {
		_shader.set3fv("material1.specular", glm::value_ptr(_renderer.mSpecularColor));
		_shader.set1i("material1.shininess", _renderer.mShininess);
	};
	auto fsLocalConfigDiffuse
		= [](const cckit::GLshader& _shader, const cckit::GLrenderer& _renderer) {
		_shader.set3fv("material.diffuse", glm::value_ptr(_renderer.mDiffuseColor));
		_shader.set3fv("material.specular", glm::value_ptr(_renderer.mSpecularColor));
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