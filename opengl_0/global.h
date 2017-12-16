#pragma once

cckit::GLcamera camera(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
glm::vec3 dirLightDir(1, -1, 1);
cckit::GLlight ptLights[1]{ cckit::GLlight(glm::vec3(0, 0, -2), glm::vec3(1, 1, 1)) };