#ifndef CCKIT_GL_INPUT.H
#define CCKIT_GL_INPUT

#include <GLFW/glfw3.h>

namespace cckit 
{
	bool glGetKeyDown(GLFWwindow* _pWindow, int _key) {
		static bool bKeyPressed = false;
		if (glfwGetKey(_pWindow, _key) == GLFW_PRESS
			&& !bKeyPressed) {
			bKeyPressed = true;
			return true;
		}
		else if (glfwGetKey(_pWindow, _key) == GLFW_RELEASE)
			bKeyPressed = false;
		return false;
	}
}

#endif // !CCKIT_GL_INPUT.H