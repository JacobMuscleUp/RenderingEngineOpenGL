#include <iostream>
#include <cassert>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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
	
	glEnable(GL_DEPTH_TEST);

	pShaderCoordAxes = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderOutline = cckit::GLfactory<cckit::GLshader>::generate();
	pShaderCoordAxes->load("Shaders/shaderMonoColor0.vs", "Shaders/shaderMonoColor0.fs");
	pShaderOutline->load("Shaders/shaderMonoColor1.vs", "Shaders/shaderMonoColor1.fs");
	setup_fsConfigs();

	cckit::GLfactory<cckit::GLobj> objFactory;
	cckit::GLobj& spider = cckit::GenPrefabSpider(cckit::ConfigPrefabSpider0);
	cckit::GLobj& bull = cckit::GenPrefabBull(cckit::ConfigPrefabBull0);
	cckit::GLobj& lamp = cckit::GenPrefabLamp(cckit::ConfigPrefabLamp0);
	cckit::GLobj& spawner = cckit::GenPrefabBullSpawner(cckit::ConfigPrefabBullSpawner0);
	lampBehavior = *lamp.get_behavior<cckit::BehaviorLamp>();
	
	float deltaTime
		, lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(_pWindow)) {
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
		process_keyboard(_pWindow, camera, deltaTime);

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
	}

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
}

GLuint GenFrameBuffer() {
	GLuint framebufferHandle;
	glGenFramebuffers(1, &framebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);
	// color attachment
	GLuint texturebufferHandle;
	glGenTextures(1, &texturebufferHandle);
	glBindTexture(GL_TEXTURE_2D, texturebufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texturebufferHandle, 0);
	// depth and stencil attachment
	GLuint renderbufferHandle;
	glGenRenderbuffers(1, &renderbufferHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferHandle);
	// verify framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "framebuffer incomplete" << endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return framebufferHandle;
}

void setup_fsConfigs() {
	cckit::GLshader::mMapShaderPath2FsConfig
		[cckit::GLshader::mStringHash("Shaders/shader0.vs")]
		[cckit::GLshader::mStringHash("Shaders/shader0.fs")]
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

	cckit::GLshader::mMapShaderPath2FsConfig
		[cckit::GLshader::mStringHash("Shaders/shader1.vs")]
		[cckit::GLshader::mStringHash("Shaders/shader1.fs")]
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
	
	cckit::GLshader::mMapShaderPath2FsConfig
		[cckit::GLshader::mStringHash("Shaders/shaderLamp.vs")]
		[cckit::GLshader::mStringHash("Shaders/shaderLamp.fs")]
	= [](const cckit::GLshader&) {}; 
}