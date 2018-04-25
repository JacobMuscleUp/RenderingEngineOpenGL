#ifndef CCKIT_GLCAMERA_H
#define CCKIT_GLCAMERA_H

#include <glad/glad.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLshader.h"
#include "GLutils.h"
#include "GLobj.h"
#include "GLmatrixTransform.h"
#include "GLgeometry.h"

namespace cckit
{
	class GLcamera
	{
	public:
		enum class movement {
			forward, backward, left, right, count
		};
		enum class projection {
			viewSpace, lightSpace, count
		};
		static const float YAW
			, PITCH
			, MOVE_SPEED
			, ROTATE_SPEED
			, ZOOM_SPEED;
	public:
		GLcamera(const glm::vec3& _pos, const glm::vec3& _worldUp, float _yaw = YAW, float _pitch = PITCH
			, float _moveSpeed = MOVE_SPEED, float _rotateSpeed = ROTATE_SPEED, float _zoomSpeed = ZOOM_SPEED)
			: mPos(_pos), mWorldUp(_worldUp), mForward(glm::vec3(0, 0, -1)), mYaw(_yaw), mPitch(_pitch)
			, mMoveSpeed(_moveSpeed), mRotateSpeed(_rotateSpeed), mZoomSpeed(_zoomSpeed)
			, mpShaderOutline(nullptr), mpShaderCoordAxes(nullptr)
			, mMatProjection() {
			UpdateCoordAxes();
		}

		glm::mat4 get_view_matrix() const {
			return glm::lookAtRH(mPos, mPos + mForward);
			return glm::lookAt(mPos, mPos + mForward, mUp);
		}

		void process_keyboard(movement _movement, float _deltaTime) {
			float moveSpeed = mMoveSpeed * _deltaTime;
			if (_movement == movement::backward)
				mPos -= mForward * moveSpeed;
			else if (_movement == movement::forward)
				mPos += mForward * moveSpeed;
			else if (_movement == movement::left)
				mPos -= mRight * moveSpeed;
			else if (_movement == movement::right)
				mPos += mRight * moveSpeed;
		}

		void process_mouse(float _offsetX, float _offsetY) {
			mYaw += _offsetX * mRotateSpeed;
			mPitch += _offsetY * mRotateSpeed;
			mPitch = max(-89.0f, min(mPitch, 89.0f));
			UpdateCoordAxes();
		}

		void set_shader_outline(const GLshader& _shader);
		void set_shader_coord_axes(const GLshader& _shader);
		template<projection>
		void set_perspective(GLfloat _fov, GLfloat _aspect, GLfloat _near, GLfloat _far);
		template<>
		void set_perspective<projection::viewSpace>(GLfloat _fov, GLfloat _aspect, GLfloat _near, GLfloat _far);
		template<>
		void set_perspective<projection::lightSpace>(GLfloat _fov, GLfloat _aspect, GLfloat _near, GLfloat _far);
		
		template<projection>
		void set_ortho(GLfloat _left, GLfloat _right, GLfloat _bottom, GLfloat _top, GLfloat _near, GLfloat _far);
		template<>
		void set_ortho<projection::viewSpace>(GLfloat _left, GLfloat _right, GLfloat _bottom, GLfloat _top, GLfloat _near, GLfloat _far);
		template<>
		void set_ortho<projection::lightSpace>(GLfloat _left, GLfloat _right, GLfloat _bottom, GLfloat _top, GLfloat _near, GLfloat _far);

		void render(const GLobj& _obj, GLenum _renderMode = GL_FILL) const;
		void render(const GLobj& _obj, const glm::mat4& _matView, GLenum _renderMode = GL_FILL) const;
		void render2depthMap(const GLobj& _obj, const glm::mat4& _matView, GLshader& _shaderDepthMap) const;

		void render(const GLprimitive& _primitive) const;

		const glm::vec3& pos() const { return mPos; }
		const glm::vec3& forward() const { return mForward; }
		const glm::vec3& right() const { return mRight; }
		const glm::vec3& up() const { return mUp; }
	private:
		void UpdateCoordAxes() {
			glm::vec3 forward(
				cos(glm::radians(mYaw)) * cos(glm::radians(mPitch))
				, sin(glm::radians(mPitch))
				, sin(glm::radians(mYaw)) * cos(glm::radians(mPitch))
			);
			mForward = glm::normalize(forward);
			mRight = glm::normalize(glm::cross(mForward, mWorldUp));
			mUp = glm::normalize(glm::cross(mRight, mForward));
		}
	private:
		glm::vec3 mPos, mForward, mRight, mUp, mWorldUp;
		float mYaw, mPitch;
		float mMoveSpeed, mRotateSpeed, mZoomSpeed;

		const GLshader* mpShaderOutline;
		const GLshader* mpShaderCoordAxes;
		glm::mat4 mMatProjection;
		glm::mat4 mMatProjectionLightSpace;
	};
	const float GLcamera::YAW = 0.0f;
	const float GLcamera::PITCH = 0.0f;
	const float GLcamera::MOVE_SPEED = 1.0f;
	const float GLcamera::ROTATE_SPEED = 1.0f;
	const float GLcamera::ZOOM_SPEED = 1.0f;

	void GLcamera::set_shader_outline(const GLshader& _shader) {
		mpShaderOutline = &_shader;
	}

	void GLcamera::set_shader_coord_axes(const GLshader& _shader) {
		mpShaderCoordAxes = &_shader;
	}

	template<>
	void GLcamera::set_perspective<GLcamera::projection::viewSpace>(GLfloat _fov, GLfloat _aspect, GLfloat _near, GLfloat _far) {
		mMatProjection = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
	}
	template<>
	void GLcamera::set_perspective<GLcamera::projection::lightSpace>(GLfloat _fov, GLfloat _aspect, GLfloat _near, GLfloat _far) {
		mMatProjectionLightSpace = glm::perspective(glm::radians(_fov), _aspect, _near, _far);
	}

	template<>
	void GLcamera::set_ortho<GLcamera::projection::viewSpace>(GLfloat _left, GLfloat _right, GLfloat _bottom, GLfloat _top, GLfloat _near, GLfloat _far) {
		mMatProjection = glm::ortho(_left, _right, _bottom, _top, _near, _far);
	}
	template<>
	void GLcamera::set_ortho<GLcamera::projection::lightSpace>(GLfloat _left, GLfloat _right, GLfloat _bottom, GLfloat _top, GLfloat _near, GLfloat _far) {
		mMatProjectionLightSpace = glm::ortho(_left, _right, _bottom, _top, _near, _far);
	}

	void GLcamera::render(const GLobj& _obj, GLenum _renderMode) const {
		render(_obj, get_view_matrix(), _renderMode);
	}

	void GLcamera::render(const GLobj& _obj, const glm::mat4& _matView, GLenum _renderMode) const {
		if (!_obj.mpShader || !_obj.behaviors_started() || _obj.destroyed()) return;

		_obj.PrepareRenderStates([&_obj](glm::mat4& _mat) {
			_mat = glm::scale(_mat, glm::vec3(_obj.mOutlineScale));
		});

		const glm::mat4& matModel = _obj.mMatModel
			, matOutlineModel = _obj.mMatOutlineModel;
		glm::mat4 matNormalModel = glm::transpose(glm::inverse(matModel));
		// render model and outline
		glPolygonMode(GL_FRONT_AND_BACK, _renderMode);
		_obj.RenderModel(
			*_obj.mpShader
			, [&](const GLshader& _shader) {
			_shader.use();
			_shader.setmatrix4fv("matModel", 1, GL_FALSE, matModel);
			_shader.setmatrix4fv("matView", 1, GL_FALSE, _matView);
			_shader.setmatrix4fv("matProjection", 1, GL_FALSE, mMatProjection);
			_shader.setmatrix4fv("matNormalModel", 1, GL_FALSE, matNormalModel);
			_shader.setmatrix4fv("matProjectionLightSpace", 1, GL_FALSE, mMatProjectionLightSpace);
			_shader.mFsGlobalConfig(_shader);
		}
			, mpShaderOutline
			, [&](const GLshader& _shader) {
			_shader.use();
			_shader.setmatrix4fv("matModel", 1, GL_FALSE, matOutlineModel);
			_shader.setmatrix4fv("matView", 1, GL_FALSE, _matView);
			_shader.setmatrix4fv("matProjection", 1, GL_FALSE, mMatProjection);
			_shader.set3fv("monoColor", _obj.mOutlineColor);
		});
		// render coord axes
		_obj.RenderCoordAxes(*mpShaderCoordAxes, [&](const GLshader& _shader) {
			_shader.use();
			_shader.setmatrix4fv("matModel", 1, GL_FALSE, glm::mat4());
			_shader.setmatrix4fv("matView", 1, GL_FALSE, _matView);
			_shader.setmatrix4fv("matProjection", 1, GL_FALSE, mMatProjection);
		});
	}

	void GLcamera::render2depthMap(const GLobj& _obj, const glm::mat4& _matView, GLshader& _shaderDepthMap) const {
		if (!_obj.mpShader || !_obj.behaviors_started() || _obj.destroyed()) return;

		_obj.PrepareRenderStates([&_obj](glm::mat4& _mat) {
			_mat = glm::scale(_mat, glm::vec3(_obj.mOutlineScale));
		});

		const glm::mat4& matModel = _obj.mMatModel
			, matOutlineModel = _obj.mMatOutlineModel;
		// render model and outline
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		_obj.RenderModel(
			_shaderDepthMap
			, [&](const GLshader& _shader) {
			_shader.use();
			_shader.setmatrix4fv("matModel", 1, GL_FALSE, matModel);
			_shader.setmatrix4fv("matView", 1, GL_FALSE, _matView);
			_shader.setmatrix4fv("matProjection", 1, GL_FALSE, mMatProjection);
		}
			, mpShaderOutline
			, [&](const GLshader& _shader) {
			_shader.use();
			_shader.setmatrix4fv("matModel", 1, GL_FALSE, matOutlineModel);
			_shader.setmatrix4fv("matView", 1, GL_FALSE, _matView);
			_shader.setmatrix4fv("matProjection", 1, GL_FALSE, mMatProjection);
			_shader.set3fv("monoColor", _obj.mOutlineColor);
		});
		// render coord axes
		_obj.RenderCoordAxes(*mpShaderCoordAxes, [&](const GLshader& _shader) {
			_shader.use();
			_shader.setmatrix4fv("matModel", 1, GL_FALSE, glm::mat4());
			_shader.setmatrix4fv("matView", 1, GL_FALSE, _matView);
			_shader.setmatrix4fv("matProjection", 1, GL_FALSE, mMatProjection);
		});
	}

	void GLcamera::render(const GLprimitive& _primitive) const {
		switch (_primitive.type()) {
		case EnumPrimitive::line:
			_primitive.render(*mpShaderCoordAxes, [&](const GLshader& _shader) {
				_shader.use();
				_shader.setmatrix4fv("matModel", 1, GL_FALSE, glm::mat4());
				_shader.setmatrix4fv("matView", 1, GL_FALSE, get_view_matrix());
				_shader.setmatrix4fv("matProjection", 1, GL_FALSE, mMatProjection);
			});
			break;
		default:
			break;
		}
	}
}

#endif // !CCKIT_GLCAMERA_H
