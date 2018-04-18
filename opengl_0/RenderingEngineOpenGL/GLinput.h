#ifndef CCKIT_GL_INPUT.H
#define CCKIT_GL_INPUT

#include <GLFW/glfw3.h>
#include <unordered_map>

namespace cckit 
{
	bool glGetKeyDown(GLFWwindow* _pWindow, int _key) {
		static std::unordered_map<int, bool> mapKey2bPressed;
		if (mapKey2bPressed.find(_key) == mapKey2bPressed.end())
			mapKey2bPressed[_key] = false;

		if (glfwGetKey(_pWindow, _key) == GLFW_PRESS
			&& !mapKey2bPressed[_key]) {
			mapKey2bPressed[_key] = true;
			return true;
		}
		else if (glfwGetKey(_pWindow, _key) == GLFW_RELEASE)
			mapKey2bPressed[_key] = false;
		return false;
	}
}

#endif // !CCKIT_GL_INPUT.H