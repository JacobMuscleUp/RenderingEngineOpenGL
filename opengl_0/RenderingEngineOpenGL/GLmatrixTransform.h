#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glm
{
	template <typename T, precision P>
	GLM_FUNC_DECL glm::tmat4x4<T, P> translate2(
		glm::tmat4x4<T, P> const & m,
		glm::tvec3<T, P> const & v)
	{
		glm::tmat4x4<T, P> result(m);

		result[0][0] = m[0][0] + m[0][3] * v[0];
		result[1][0] = m[1][0] + m[1][3] * v[0];
		result[2][0] = m[2][0] + m[2][3] * v[0];
		result[3][0] = m[3][0] + m[3][3] * v[0];

		result[0][1] = m[0][1] + m[0][3] * v[1];
		result[1][1] = m[1][1] + m[1][3] * v[1];
		result[2][1] = m[2][1] + m[2][3] * v[1];
		result[3][1] = m[3][1] + m[3][3] * v[1];

		result[0][2] = m[0][2] + m[0][3] * v[2];
		result[1][2] = m[1][2] + m[1][3] * v[2];
		result[2][2] = m[2][2] + m[2][3] * v[2];
		result[3][2] = m[3][2] + m[3][3] * v[2];

		return result;
	}

	template <typename T, precision P>
	GLM_FUNC_DECL tmat4x4<T, P> rotate2(
		tmat4x4<T, P> const & m,
		T angle,
		tvec3<T, P> const & axis)
	{
		T const c = cos(angle);
		T const s = sin(angle);

		tvec3<T, P> axisNormed = normalize(axis);
		tvec3<T, P> temp((T(1) - c) * axisNormed);

		tmat4x4<T, P> r(uninitialize);
		r[0][0] = c + temp[0] * axisNormed[0];
		r[0][1] = temp[0] * axisNormed[1] + s * axisNormed[2];
		r[0][2] = temp[0] * axisNormed[2] - s * axisNormed[1];

		r[1][0] = temp[1] * axisNormed[0] - s * axisNormed[2];
		r[1][1] = c + temp[1] * axisNormed[1];
		r[1][2] = temp[1] * axisNormed[2] + s * axisNormed[0];

		r[2][0] = temp[2] * axisNormed[0] + s * axisNormed[1];
		r[2][1] = temp[2] * axisNormed[1] - s * axisNormed[0];
		r[2][2] = c + temp[2] * axisNormed[2];

		tmat4x4<T, P> result(uninitialize);
		result[0] = r[0] * m[0][0] + r[1] * m[0][1] + r[2] * m[0][2];
		result[1] = r[0] * m[1][0] + r[1] * m[1][1] + r[2] * m[1][2];
		result[2] = r[0] * m[2][0] + r[1] * m[2][1] + r[2] * m[2][2];
		result[3] = r[0] * m[3][0] + r[1] * m[3][1] + r[2] * m[3][2];
		result[0][3] = m[0][3];
		result[1][3] = m[1][3];
		result[2][3] = m[2][3];
		result[3][3] = m[3][3];

		return result;
	}

	/*template <typename T, precision P>
	GLM_FUNC_DECL glm::tmat4x4<T, P> lookAt(
		const glm::tvec3<T, P>& _forward
		, const glm::tvec3<T, P>& _targetDir) {
		glm::tvec3<T, P> unitForward = glm::normalize(_forward)
			, unitTargetDir = glm::normalize(_targetDir)
			, rotateAxis = glm::cross(unitForward, unitTargetDir);
		GLfloat cosRadAngle = glm::dot(unitForward, unitTargetDir);
		if (cosRadAngle < 0.0f)
			rotateAxis *= -1.0f;
		GLfloat radAngle = glm::acos(cosRadAngle);

		if (glm::abs(rotateAxis[0]) < FLT_EPSILON
			&& glm::abs(rotateAxis[1]) < FLT_EPSILON
			&& glm::abs(rotateAxis[2]) < FLT_EPSILON)
			return glm::mat4();
		return glm::rotate(glm::mat4(), radAngle, rotateAxis);
	}*/

	enum class facing_mode { forward, back, left, right, bottom, top, Count };

	template <typename T, precision P>
	GLM_FUNC_DECL glm::tmat4x4<T, P> lookAt(
		const glm::tvec3<T, P>& _eyePos
		, const glm::tvec3<T, P>& _targetPos
		, facing_mode _facingMode)
	{
		glm::vec3 targetDir(glm::normalize(_targetPos - _eyePos));
		glm::vec3 up(0, 1, 0);
		if (abs(targetDir.x)< FLT_EPSILON && abs(targetDir.z) < FLT_EPSILON) 
			up = (targetDir.y < 0) ? glm::vec3(0, 0, 1) : glm::vec3(0, 0, -1);
		glm::vec3 right = glm::normalize(glm::cross(up, targetDir));
		up = glm::cross(targetDir, right);// since targetDir is perpendicular to right

		switch (_facingMode) {
		case facing_mode::forward:
			return glm::mat4(
				right.x, right.y, right.z, 0,
				up.x, up.y, up.z, 0,
				targetDir.x, targetDir.y, targetDir.z, 0,
				0, 0, 0, 1
			);
		case facing_mode::back:
			return glm::mat4(
				right.x, right.y, right.z, 0,
				up.x, up.y, up.z, 0,
				-targetDir.x, -targetDir.y, -targetDir.z, 0,
				0, 0, 0, 1
			);
		case facing_mode::right:
			return glm::mat4(
				targetDir.x, targetDir.y, targetDir.z, 0
				, up.x, up.y, up.z, 0
				, right.x, right.y, right.z, 0
				, 0, 0, 0, 1
			);
		case facing_mode::left:
			return glm::mat4(
				targetDir.x, targetDir.y, targetDir.z, 0
				, up.x, up.y, up.z, 0
				, -right.x, -right.y, -right.z, 0
				, 0, 0, 0, 1
			);
		case facing_mode::top:
			return glm::mat4(
				up.x, up.y, up.z, 0
				, targetDir.x, targetDir.y, targetDir.z, 0
				, right.x, right.y, right.z, 0
				, 0, 0, 0, 1
			);
		case facing_mode::bottom:// FIX NEEDED
			return glm::mat4(
				-up.x, -up.y, -up.z, 0
				, targetDir.x, targetDir.y, targetDir.z, 0
				, right.x, right.y, right.z, 0
				, 0, 0, 0, 1
			);
			return glm::mat4();
		}
	}
}