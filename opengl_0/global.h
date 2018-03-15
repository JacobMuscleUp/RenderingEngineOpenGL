#ifndef CCKIT_GLOBAL_H
#define CCKIT_GLOBAL_H

#include <glm/glm.hpp>
#include "RenderingEngineOpenGL/GLcamera.h"
#include "RenderingEngineOpenGL/GLlight.h"
#include "RenderingEngineOpenGL/GLshader.h"
#include "RenderingEngineOpenGL/GLfactory.h"

#define POSTPROCESS_ENABLED
//#define SHADOW_MAPPING_ENABLED

cckit::GLshader* pShaderCoordAxes;
cckit::GLshader* pShaderOutline;
cckit::GLshader* pShaderScreen;
cckit::GLshader* pShaderDepthMap;

cckit::GLcamera camera(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
glm::vec3 dirLightDir(10000, -10000, 10000);
cckit::GLlight ptLights[1]{ cckit::GLlight(glm::vec3(0, 0, -2), glm::vec3(1, 1, 1)) };

#endif // !CCKIT_GLOBAL_H