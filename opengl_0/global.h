#ifndef CCKIT_GLOBAL_H
#define CCKIT_GLOBAL_H

#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"assimp-vc140-mt.lib")

#include <glm/glm.hpp>
#include "RenderingEngineOpenGL/GLcamera.h"
#include "RenderingEngineOpenGL/GLlight.h"
#include "RenderingEngineOpenGL/GLshader.h"
#include "RenderingEngineOpenGL/GLfactory.h"

cckit::GLshader* pShaderCoordAxes;
cckit::GLshader* pShaderOutline;
cckit::GLshader* pShaderScreen;
cckit::GLshader* pShaderDepthMap;
cckit::GLshader* pShaderDepthDebug;

cckit::GLcamera camera(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
cckit::GLlight ptLights[1]{ cckit::GLlight(glm::vec3(0, 0, -2), glm::vec3(1, 1, 1)) };
glm::mat4 matViewLightSpace;
glm::vec3 dirLightDir;
glm::vec3 dirLightPos;
float orthoScale = 10.0f;
bool bDepthMapView = false;
GLint maxTextureUnits;

#endif // !CCKIT_GLOBAL_H