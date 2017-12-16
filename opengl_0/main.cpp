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

#include "Behaviors/all_behaviors.h"
#include "global.h"

using std::cout; using std::endl; using std::cin;

void process_input(GLFWwindow* _pWindow);
GLuint load_texture(const GLchar* _texturePath);

void draw_arrays(GLFWwindow* _pWindow, GLuint _programHandle);
void draw_elements(GLFWwindow* _pWindow, GLuint _programHandle, GLenum _drawMode = GL_FILL);
void draw_elements_6(GLFWwindow* _pWindow, GLuint _programHandle, GLenum _drawMode = GL_FILL);

void draw_elements_shader_prog(GLFWwindow* _pWindow, cckit::GLshader _program, GLenum _drawMode = GL_FILL);
template<GLint Width, GLint Height>
void draw_grid(GLFWwindow* _pWindow, const cckit::GLshader& _program, GLenum _drawMode = GL_LINE);

// WORKING HERE
void fps_assimp(GLFWwindow* _pWindow);
void process_keyboard(GLFWwindow* _pWindow, cckit::GLcamera& _camera, float _deltaTime, cckit::lamp& _lampBehavior);

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

	//cckit::GLshader shader("Shaders/shader0.vs", "Shaders/shader0.fs");
	//draw_grid<6, 8>(pWindow, program, GL_FILL);
	fps_assimp(pWindow);
	
	glfwTerminate();
	cin >> temp;
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void process_input(GLFWwindow* _pWindow)
{
	if (glfwGetKey(_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_pWindow, true);
	}
}

void process_input_0(GLFWwindow* _pWindow, float* _pTranslationX, float* _pTranslationZ, float* _pRotationY)
{
	if (glfwGetKey(_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_pWindow, true);
	}
	else if (glfwGetKey(_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
		*_pTranslationX += 0.01f;
	}
	else if (glfwGetKey(_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		*_pTranslationX -= 0.01f;
	}
	else if (glfwGetKey(_pWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
		*_pTranslationZ -= 0.01f;
	}
	else if (glfwGetKey(_pWindow, GLFW_KEY_UP) == GLFW_PRESS) {
		*_pTranslationZ += 0.01f;
	}
	else if (glfwGetKey(_pWindow, GLFW_KEY_A) == GLFW_PRESS) {
		*_pRotationY -= 1.0f;
	}
	else if (glfwGetKey(_pWindow, GLFW_KEY_D) == GLFW_PRESS) {
		*_pRotationY += 1.0f;
	}
}

GLuint load_texture(const GLchar* _texturePath)
{
	GLuint textureHandle;

	glGenTextures(1, &textureHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

	int width, height, channelCount;
	unsigned char* textureData = stbi_load(_texturePath, &width, &height, &channelCount, 0);
	if (textureData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "texture failed" << endl;
	}
	stbi_image_free(textureData);

	return textureHandle;
}

void draw_arrays(GLFWwindow* _pWindow, GLuint _programHandle)
{
	float vertices[] = {
		1.0f, 0.5f, 0.0f
		, 0.5f, -0.5f, 0.0f
		, -0.5f, -0.5f, 0.0f
	};

	GLuint vboHandle, vaoHandle;
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &vboHandle);
	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(_pWindow)) {
		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
		process_input(_pWindow);

		glClearColor(1.0f, 0.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat greenVal = sin(glfwGetTime()) * 0.5f + 0.5f;
		GLint sharedColorIndex = glGetUniformLocation(_programHandle, "sharedColor");
		glUseProgram(_programHandle);
		glUniform4f(sharedColorIndex, 0.0f, greenVal, 0.0f, 1.0f);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	glDeleteVertexArrays(1, &vaoHandle);
	glDeleteBuffers(1, &vboHandle);
}

void draw_elements(GLFWwindow* _pWindow, GLuint _programHandle, GLenum _drawMode)
{
	GLfloat vertices[] = {
		1.0f, 0.5f, 0.0f// top right
		, 0.5f, -0.5f, 0.0f// bottom right
		, -0.5f, -0.5f, 0.0f// bottom left
		, 0.0f, 0.5f, 0.0f// top left
	};
	GLint indices[] = {
		0, 1, 3
		, 1, 2, 3
	};

	GLuint veoHandle, vboHandle, vaoHandle;
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &veoHandle);
	glGenBuffers(1, &vboHandle);

	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, _drawMode);

	while (!glfwWindowShouldClose(_pWindow)) {
		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
		process_input(_pWindow);

		glClearColor(1.0f, 0.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat greenVal = sin(glfwGetTime()) * 0.5f + 0.5f;
		GLint sharedColorIndex = glGetUniformLocation(_programHandle, "sharedColor");
		glUseProgram(_programHandle);
		glUniform4f(sharedColorIndex, 0.0f, greenVal, 1.0f, 1.0f);

		glDrawElements(GL_TRIANGLES, cckit::length(indices), GL_UNSIGNED_INT, nullptr);
	}

	glDeleteVertexArrays(1, &vaoHandle);
	glDeleteBuffers(1, &veoHandle);
	glDeleteBuffers(1, &vboHandle);
}

void draw_elements_6(GLFWwindow* _pWindow, GLuint _programHandle, GLenum _drawMode)
{
	GLfloat vertices[] = {
		1.0f, 0.5f, 0.0f  , 1.0f, 0.0f, 0.0f// top right
		, 0.5f, -0.5f, 0.0f  , 0.0f, 1.0f, 0.0f// bottom right
		, -0.5f, -0.5f, 0.0f  , 0.0f, 0.0f, 1.0f// bottom left
		, 0.0f, 0.5f, 0.0f  , 1.0f, 1.0f, 1.0f// top left
	};
	GLint indices[] = {
		0, 1, 3
		, 1, 2, 3
	};

	GLuint veoHandle, vboHandle, vaoHandle;
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &veoHandle);
	glGenBuffers(1, &vboHandle);

	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glPolygonMode(GL_FRONT_AND_BACK, _drawMode);

	while (!glfwWindowShouldClose(_pWindow)) {
		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
		process_input(_pWindow);

		glClearColor(1.0f, 0.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat greenVal = sin(glfwGetTime()) * 0.5f + 0.5f;
		GLint sharedColorIndex = glGetUniformLocation(_programHandle, "sharedColor");
		glUseProgram(_programHandle);
		glUniform4f(sharedColorIndex, 0.0f, greenVal, 1.0f, 1.0f);

		glDrawElements(GL_TRIANGLES, cckit::length(indices), GL_UNSIGNED_INT, nullptr);
	}

	glDeleteVertexArrays(1, &vaoHandle);
	glDeleteBuffers(1, &veoHandle);
	glDeleteBuffers(1, &vboHandle);
}

void draw_elements_shader_prog(GLFWwindow* _pWindow, cckit::GLshader _program, GLenum _drawMode)
{
	GLfloat vertices[] = {
		1.0f, 0.5f, 0.0f  , 1.0f, 0.0f, 0.0f// top right
		, 0.5f, -0.5f, 0.0f  , 0.0f, 1.0f, 0.0f// bottom right
		, -0.5f, -0.5f, 0.0f  , 0.0f, 0.0f, 1.0f// bottom left
		, 0.0f, 0.5f, 0.0f  , 1.0f, 1.0f, 1.0f// top left
	};
	GLint indices[] = {
		0, 1, 3
		, 1, 2, 3
	};

	GLuint veoHandle, vboHandle, vaoHandle;
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &veoHandle);
	glGenBuffers(1, &vboHandle);

	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glPolygonMode(GL_FRONT_AND_BACK, _drawMode);

	while (!glfwWindowShouldClose(_pWindow)) {
		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
		process_input(_pWindow);

		glClearColor(1.0f, 0.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat greenVal = sin(glfwGetTime()) * 0.5f + 0.5f;
		_program.use();
		_program.set4f("sharedColor", 0.0f, greenVal, 1.0f, 1.0f);

		glDrawElements(GL_TRIANGLES, cckit::length(indices), GL_UNSIGNED_INT, nullptr);
	}

	glDeleteVertexArrays(1, &vaoHandle);
	glDeleteBuffers(1, &veoHandle);
	glDeleteBuffers(1, &vboHandle);
}

template<GLint Width, GLint Height>
void draw_grid(GLFWwindow* _pWindow, const cckit::GLshader& _program, GLenum _drawMode)
{
	GLfloat vertices[8 * Width * Height];
	GLint indices[6 * (Width - 1) * (Height - 1)];
	GLfloat origin[3] = { -6.0f, 6.0f, -10.0f };

	for (GLint y = 0; y < Height; ++y) {
		for (GLint x = 0; x < Width; ++x) {
			vertices[8 * (x + y * Width)] = origin[0] + static_cast<GLfloat>(x);
			vertices[8 * (x + y * Width) + 1] = origin[1] + static_cast<GLfloat>(-y);
			vertices[8 * (x + y * Width) + 2] = origin[2] + 0.0f;
			vertices[8 * (x + y * Width) + 3] = 0.0f;
			vertices[8 * (x + y * Width) + 4] = 1.0f;
			vertices[8 * (x + y * Width) + 5] = 1.0f;
			vertices[8 * (x + y * Width) + 6] = static_cast<GLfloat>(x) / Width;
			vertices[8 * (x + y * Width) + 7] = static_cast<GLfloat>(y) / Height;
		}
	}
	/*GLfloat vertices[3 * Width * Height];
	GLint indices[6 * (Width - 1) * (Height - 1)];
	GLfloat origin[3] = { -6.0f, 6.0f, 0.0f };

	for (GLint y = 0; y < Height; ++y) {
		for (GLint x = 0; x < Width; ++x) {
			vertices[3 * (x + y * Width)] = origin[0] + static_cast<GLfloat>(x);
			vertices[3 * (x + y * Width) + 1] = origin[1] + static_cast<GLfloat>(-y);
			vertices[3 * (x + y * Width) + 2] = origin[2] + 0.0f;
		}
	}*/

	for (GLint y = 0; y < Height - 1; ++y) {
		for (GLint x = 0; x < Width - 1; ++x) {
			indices[6 * (x + y * (Width - 1))] = x + y * Width + 1;
			indices[6 * (x + y * (Width - 1)) + 1] = x + (y + 1) * Width + 1;
			indices[6 * (x + y * (Width - 1)) + 2] = x + y * Width;
			indices[6 * (x + y * (Width - 1)) + 3] = x + (y + 1) * Width + 1;
			indices[6 * (x + y * (Width - 1)) + 4] = x + (y + 1) * Width;
			indices[6 * (x + y * (Width - 1)) + 5] = x + y * Width;
		}
	}

	GLuint eboHandle, vboHandle, vaoHandle;
	glGenVertexArrays(1, &vaoHandle);
	glGenBuffers(1, &eboHandle);
	glGenBuffers(1, &vboHandle);

	glBindVertexArray(vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	//glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glPolygonMode(GL_FRONT_AND_BACK, _drawMode);

	//load_texture(_program, "Resources/bricks2.jpg");

	while (!glfwWindowShouldClose(_pWindow)) {
		static float translationX = 0.5f;
		static float translationZ = -4.0f;
		static float rotationY = 0.0f;

		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
		process_input_0(_pWindow, &translationX, &translationZ, &rotationY);

		glClearColor(1.0f, 0.0f, 1.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat greenVal = sin(glfwGetTime()) * 0.5f + 0.5f;
		_program.use();
		_program.set4f("sharedColor", 0.0f, greenVal, 1.0f, 1.0f);

		glm::mat4 modelMat;
		modelMat = glm::rotate(modelMat, 30 * glm::radians(static_cast<float>(glfwGetTime())), glm::vec3(0, 1, 0));
		modelMat = glm::rotate(modelMat, glm::radians(static_cast<float>(-90)), glm::vec3(1, 0, 0));
		modelMat = glm::scale(modelMat, glm::vec3(0.1f, 0.1f, 0.1f));
		glm::mat4 viewMat;
		viewMat = glm::rotate(viewMat, glm::radians(static_cast<float>(rotationY)), glm::vec3(0, 1, 0));
		viewMat = glm::translate(viewMat, glm::vec3(translationX, 0, translationZ));
		glm::mat4 projectionMat;
		projectionMat = glm::perspective(glm::radians(static_cast<float>(45)), static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 1000.0f);
		_program.setmatrix4fv("modelMat", 1, GL_FALSE, glm::value_ptr(modelMat));
		_program.setmatrix4fv("viewMat", 1, GL_FALSE, glm::value_ptr(viewMat));
		_program.setmatrix4fv("projectionMat", 1, GL_FALSE, glm::value_ptr(projectionMat));

		glDrawElements(GL_TRIANGLES, length(indices), GL_UNSIGNED_INT, nullptr);
	}

	glDeleteVertexArrays(1, &vaoHandle);
	glDeleteBuffers(1, &eboHandle);
	glDeleteBuffers(1, &vboHandle);
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
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

	cckit::GLshader shaderTexture("Shaders/shader0.vs", "Shaders/shader0.fs");
	cckit::GLshader shaderDiffuse("Shaders/shader1.vs", "Shaders/shader1.fs");
	cckit::GLshader shaderLamp("Shaders/shaderLamp.vs", "Shaders/shaderLamp.fs");
	cckit::GLshader shaderCoordAxes("Shaders/shaderMonoColor0.vs", "Shaders/shaderMonoColor0.fs");
	cckit::GLshader shaderOutline("Shaders/shaderMonoColor1.vs", "Shaders/shaderMonoColor1.fs");

	cckit::GLobj spider("Resources/OBJ/spider/spider.obj");
	cckit::GLobj bull("Resources/OBJ/bull/bull.obj");
	cckit::GLobj lamp("Resources/OBJ/box/box.obj");

	spider.add_behavior(cckit::spider());
	bull.add_behavior(cckit::bull());
	lamp.add_behavior(cckit::lamp([](cckit::lamp& _behavior) {
		_behavior.mStartingPos = ptLights[0].mPos;
	}));
	cckit::lamp& lampBehavior = *lamp.get_behavior<cckit::lamp>();

	for (auto pObj : cckit::GLobj::Objs())
		pObj->start_behaviors();// start

	float deltaTime
		, lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(_pWindow)) {
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		glfwSwapBuffers(_pWindow);
		glfwPollEvents();
		process_keyboard(_pWindow, camera, deltaTime, lampBehavior);

		glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shaderTexture.mShaderConfig = [&lampBehavior](const cckit::GLshader& _shader) {
			glm::vec3 dirLightDiffuse = ptLights[0].mColor * glm::vec3(0.7f)
				, dirLightAmbient = dirLightDiffuse * glm::vec3(0.2f)
				, dirLightSpecular = glm::vec3(1.0f);
			glm::vec3 ptLightDiffuse = ptLights[0].mColor * glm::vec3(0.7f)
				, ptLightAmbient = dirLightDiffuse * glm::vec3(0.2f)
				, ptLightSpecular = glm::vec3(1.0f);
			glm::vec3 spotLightDiffuse = glm::vec3(0, 1, 0)
				, spotLightAmbient = dirLightDiffuse * glm::vec3(0.2f)
				, spotLightSpecular = glm::vec3(1.0f);

			_shader.set3fv("material1.diffuse", glm::value_ptr(glm::vec3(0, 1, 0)));
			_shader.set3fv("material1.specular", glm::value_ptr(glm::vec3(1.0f)));
			_shader.set1i("material1.shininess", 32);

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
		shaderLamp.mShaderConfig = shaderDiffuse.mShaderConfig = shaderTexture.mShaderConfig;

		for (auto pObj : cckit::GLobj::Objs())
			pObj->update_behaviors(deltaTime);// update
		
		camera.set_perspective(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);
		camera.set_shader_outline(shaderOutline);
		camera.set_shader_coord_axes(shaderCoordAxes);

		camera.set_shader(shaderTexture);
		camera.render(spider);
		camera.set_shader(shaderDiffuse);
		camera.render(bull);
		camera.set_shader(shaderLamp);
		camera.render(lamp); 
	}
}

void process_keyboard(GLFWwindow* _pWindow, cckit::GLcamera& _camera, float _deltaTime, cckit::lamp& _lampBehavior)
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
		_lampBehavior.obj().set_position(_lampBehavior.obj().position() + glm::vec3(0, 0, _deltaTime));
	else if (glfwGetKey(_pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
		_lampBehavior.obj().set_position(_lampBehavior.obj().position() - glm::vec3(0, 0, _deltaTime));
	if (glfwGetKey(_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
		_lampBehavior.obj().set_position(_lampBehavior.obj().position() + glm::vec3(_deltaTime, 0, 0));
	else if (glfwGetKey(_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
		_lampBehavior.obj().set_position(_lampBehavior.obj().position() - glm::vec3(_deltaTime, 0, 0));
}