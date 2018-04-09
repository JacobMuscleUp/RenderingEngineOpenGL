#ifndef CCKIT_GLOBAL_H
#define CCKIT_GLOBAL_H

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
//glm::vec3 dirLightDir(10000, -10000, 10000);
cckit::GLlight ptLights[1]{ cckit::GLlight(glm::vec3(0, 0, -2), glm::vec3(1, 1, 1)) };
glm::mat4 lightSpaceMatrix;
glm::vec3 dirLightDir;

#endif // !CCKIT_GLOBAL_H