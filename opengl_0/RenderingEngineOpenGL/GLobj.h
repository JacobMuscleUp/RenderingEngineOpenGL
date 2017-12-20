#ifndef CCKIT_GLOBJ_H
#define CCKIT_GLOBJ_H

#include <cfloat>
#include <unordered_set>
#include <stack>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLbase.h"
#include "GLfactory.h"
#include "GLrenderer.h"
#include "GLbehavior.h"
#include "GLlight.h"
#include "GLmatrixTransform.h"

#define DEFAULT_OUTLINE_SCALE 1.02f

#define FORWARD_AXIS glm::vec3(0, 0, 1)
#define RIGHT_AXIS glm::vec3(1, 0, 0)
#define UP_AXIS glm::vec3(0, 1, 0)

#define FORWARD_AXIS_COLOR  glm::vec3(1, 0, 0)
#define RIGHT_AXIS_COLOR  glm::vec3(0, 1, 0)
#define UP_AXIS_COLOR  glm::vec3(0, 0, 1)

#define DEFAULT_OUTLINE_COLOR glm::vec3(1, 0, 0)

namespace cckit
{
	class GLcamera;

	class GLobj : public GLbase
	{
	private:
		static glm::vec3 M_FORWARD_AXIS;
		static glm::vec3 M_RIGHT_AXIS;
		static glm::vec3 M_UP_AXIS;
		static glm::vec3 M_DEFAULT_OUTLINE_COLOR;
	private:
		GLobj();
		explicit GLobj(const std::string& _modelPath);
	public:
		~GLobj();

		bool load_model(const std::string& _modelPath);

		const glm::mat4& model_mat() const { return mModelMat; }
		GLmodel* model_ptr() const { return mpRenderer->mpModel; }
		GLrenderer* renderer_ptr() const { return mpRenderer; }

		const glm::vec3& position() const { return mPosition; }
		void set_position(const glm::vec3& _position) {
			mPosition = mUnmodifiedPosition = _position;
		}

		void set_shader(GLshader& _shader);
		GLshader& shader() const { return *mpShader; }
		void apply_renderer_config();

		void face(const glm::vec3& _targetPos, glm::facing_mode _facingMode = glm::facing_mode::forward);

		const glm::vec3& forward() const { return mForward; }
		const glm::vec3& right() const { return mRight; }
		const glm::vec3& up() const { return mUp; }

		bool add_behavior(GLbehavior* _pBehavior);
		bool remove_behavior(GLbehavior* _pBehavior);
		void clear_behaviors();
		void start_behaviors();
		void update_behaviors(float _deltaTime);
		bool behaviors_started() const { return mbBehaviorsStarted; }
		bool destroyed() const { return mbDestroyed; }

		template<typename BehaviorType>
		BehaviorType* get_behavior() const;

		void destroy() { mbDestroyed = true; }

		// invocation sequence: globally_start_behaviors => globally_update_behaviors => globally_render
		static void globally_start_behaviors();
		static void globally_update_behaviors(float _deltaTime);
		static void globally_render(GLcamera* _pCamera
			, std::function<void(GLcamera&)> _cameraConfig, std::function<void(GLcamera&, const GLobj&)> _cameraRender);
		//! invocation sequence

		static const std::unordered_set<GLobj*>& Objs() { return mObjs; }
	private:
		void PrepareRenderStates(std::function<void(glm::mat4&)> _outlineModelMatConfig) const;
		void RenderModel(const GLshader& _shader, std::function<void(const GLshader&)> _uniformConfig
			, const GLshader* _pShaderOutline = nullptr
			, std::function<void(const GLshader&)> _uniformConfigOutline = [](const GLshader&) {}) const;
		void RenderCoordAxes(const GLshader& _shader
			, std::function<void(const GLshader&)> _uniformConfig = [](const GLshader&) {}) const;

	public:
		glm::vec3 mLocalPosition;
		glm::vec3 mlocalRotation;
		glm::vec3 mLocalScale;
		glm::vec3 mRotation;
		glm::vec3 mScale;

		GLfloat mOutlineScale;
		GLfloat mCoordAxisLength;
		bool mbOutlined;
		bool mbCoordAxesDrawn;
		glm::vec3 mOutlineColor;
		glm::vec3 mAxisColors[3];// forward, right, up

		std::function<void(glm::mat4&)> mTranslateFunc;
		std::function<void(glm::mat4&)> mRotateFunc;
		std::function<void(glm::mat4&)> mScaleFunc;
	private:
		mutable glm::vec3 mPosition;
		mutable glm::vec3 mUnmodifiedPosition;

		mutable glm::vec3 mForward;
		mutable glm::vec3 mRight;
		mutable glm::vec3 mUp;
	private:
		GLrenderer* mpRenderer;
		glm::vec3 mLocalCenter;
		GLshader* mpShader;
		mutable bool mbFacingTarget;
		glm::vec3 mTargetPos;
		glm::facing_mode mFacingMode;

		mutable glm::mat4 mModelMat;
		mutable glm::mat4 mOutlineModelMat;
		mutable GLfloat mCoordAxes1D[54];// 6:3:3 <=> vertAttribs : verts : coordAxes

		std::unordered_set<GLbehavior*> mBehaviors;
		mutable bool mbBehaviorsStarted;
		bool mbDestroyed;

		static std::unordered_set<GLobj*> mObjs;

		friend GLcamera;
		friend GLfactory<GLobj>;
	};
	glm::vec3 GLobj::M_FORWARD_AXIS = FORWARD_AXIS;
	glm::vec3 GLobj::M_RIGHT_AXIS = RIGHT_AXIS;
	glm::vec3 GLobj::M_UP_AXIS = UP_AXIS;
	glm::vec3 GLobj::M_DEFAULT_OUTLINE_COLOR = DEFAULT_OUTLINE_COLOR;
	std::unordered_set<GLobj*> GLobj::mObjs;

	inline GLobj::GLobj()
		: mpRenderer(nullptr), mbFacingTarget(false), mModelMat()
		, mLocalPosition(0.0f), mlocalRotation(0.0f), mLocalScale(1.0f)
		, mPosition(0.0f), mRotation(0.0f), mScale(1.0f)
		, mForward(M_FORWARD_AXIS[0], M_FORWARD_AXIS[1], M_FORWARD_AXIS[2])
		, mRight(M_RIGHT_AXIS[0], M_RIGHT_AXIS[1], M_RIGHT_AXIS[2])
		, mUp(M_UP_AXIS[0], M_UP_AXIS[1], M_UP_AXIS[2])
		, mOutlineScale(DEFAULT_OUTLINE_SCALE), mCoordAxisLength(1.0f)
		, mbOutlined(false), mbCoordAxesDrawn(false)
		, mOutlineColor(M_DEFAULT_OUTLINE_COLOR)
		, mTranslateFunc([](glm::mat4&) {}), mRotateFunc([](glm::mat4&) {}), mScaleFunc([](glm::mat4&) {})
		, mBehaviors(), mbBehaviorsStarted(false), mbDestroyed(false)
	{
		::new(&mAxisColors[0]) glm::vec3(FORWARD_AXIS_COLOR);
		::new(&mAxisColors[1]) glm::vec3(RIGHT_AXIS_COLOR);
		::new(&mAxisColors[2]) glm::vec3(UP_AXIS_COLOR);

		for (int i = length(mCoordAxes1D) / 6 - 1; i >= 0; --i) {
			size_t axisIndex = i / 3;
			mCoordAxes1D[6 * i + 3] = mAxisColors[axisIndex][0];
			mCoordAxes1D[6 * i + 4] = mAxisColors[axisIndex][1];
			mCoordAxes1D[6 * i + 5] = mAxisColors[axisIndex][2];
		}

		mObjs.insert(this);
	}

	GLobj::GLobj(const std::string& _modelPath)
		: GLobj() {
		load_model(_modelPath);
	}

	inline GLobj::~GLobj() {
		for (auto pBehavior : mBehaviors) {
			pBehavior->on_destroyed();
			delete pBehavior;
		}
		if (mpRenderer)
			delete mpRenderer;
		mObjs.erase(this);
		std::cout << "obj destroyed" << "\n";
	}

	bool GLobj::load_model(const std::string& _modelPath) {
		if (mpRenderer && model_ptr()) 
			delete model_ptr();
		mpRenderer = new GLrenderer(_modelPath);
		if (!model_ptr()) 
			return false;

		GLfloat xMin = FLT_MAX, xMax = FLT_MIN
			, yMin = FLT_MAX, yMax = FLT_MIN
			, zMin = FLT_MAX, zMax = FLT_MIN;

		for (GLmesh* pMesh : model_ptr()->mMeshes) {
			for (const GLvertex& vertex : pMesh->mVertices) {
				const glm::vec3& pos = vertex.mPos;

				if (pos.x < xMin)
					xMin = pos.x;
				else if (xMax < pos.x)
					xMax = pos.x;
				if (pos.y < yMin)
					yMin = pos.y;
				else if (yMax < pos.y)
					yMax = pos.y;
				if (pos.z < zMin)
					zMin = pos.z;
				else if (zMax < pos.z)
					zMax = pos.z;
			}
		}

		mLocalCenter = glm::vec3((xMin + xMax) * 0.5f, (yMin + yMax) * 0.5f, (zMin + zMax) * 0.5f);
		return true;
	}

	inline void GLobj::set_shader(GLshader& _shader) {
		mpShader = &_shader;
	}

	void GLobj::apply_renderer_config() {
		if (mpRenderer) {
			mpShader->use();
			mpShader->set3fv("material.diffuse", glm::value_ptr(mpRenderer->mDiffuseColor));
			mpShader->set3fv("material.specular", glm::value_ptr(mpRenderer->mSpecularColor));
			mpShader->set1i("material.shininess", mpRenderer->mShininess);
			mpShader->set3fv("material1.specular", glm::value_ptr(mpRenderer->mSpecularColor));
			mpShader->set1i("material1.shininess", mpRenderer->mShininess);
		}
	}

	inline void GLobj::face(const glm::vec3& _targetPos, glm::facing_mode _facingMode) {
		mbFacingTarget = true;
		mTargetPos = _targetPos;
		mFacingMode = _facingMode;
	}

	inline bool GLobj::add_behavior(GLbehavior* _pBehavior) {
		_pBehavior->mpObj = this;
		return mBehaviors.insert(_pBehavior).second;
	}

	inline bool GLobj::remove_behavior(GLbehavior* _pBehavior) {
		size_t removedBehaviors = mBehaviors.erase(_pBehavior);
		delete _pBehavior;
		return removedBehaviors != 0;
	}

	inline void GLobj::clear_behaviors() {
		mBehaviors.clear();
	}

	inline void GLobj::start_behaviors() {
		for (auto pBehavior : mBehaviors) {
			if (!pBehavior->started())
				pBehavior->start();
			pBehavior->manage();
		}
	}

	void GLobj::update_behaviors(float _deltaTime) {
		//if (mbDestroyed)
		//	return;

		if (mbBehaviorsStarted) {
			std::stack<GLbehavior*> removedBehaviors;
			for (auto pBehavior : mBehaviors) {
				pBehavior->update(_deltaTime);
				if (mbDestroyed) 
					break;
				if (pBehavior->mbDestroyed)
					removedBehaviors.push(pBehavior);
			}
			while (!removedBehaviors.empty()) {
				remove_behavior(removedBehaviors.top());
				removedBehaviors.pop();
			}
		}
		else {
			mbBehaviorsStarted = true;
			for (auto pBehavior : mBehaviors) {
				pBehavior->update(_deltaTime);
				if (mbDestroyed)
					break;
				if (!pBehavior->started())
					mbBehaviorsStarted = false;
			}
		}

		if (mbDestroyed) {
			mObjs.erase(this);
			GLfactory<GLobj>::destroy(this);
		}
	}

	template<typename BehaviorType>
	BehaviorType* GLobj::get_behavior() const {
		for (auto pBehavior : mBehaviors) {
			auto pCastBehavior = dynamic_cast<BehaviorType*>(const_cast<GLbehavior*>(pBehavior));
			if (pCastBehavior)
				return pCastBehavior;
		}
		return nullptr;
	}

	void GLobj::globally_start_behaviors() {
		for (auto pObj : cckit::GLobj::Objs())
			pObj->start_behaviors();
	}

	void GLobj::globally_update_behaviors(float _deltaTime) {
		for (bool bNoObjDestroyed = false; !bNoObjDestroyed;) {
			bNoObjDestroyed = true;
			for (auto pObj : cckit::GLobj::Objs()) {
				pObj->update_behaviors(_deltaTime);
				if (pObj->destroyed()) {
					bNoObjDestroyed = false;
					break;
				}
			}
		}
	}

	void GLobj::globally_render(GLcamera* _pCamera
		, std::function<void(GLcamera&)> _cameraConfig, std::function<void(GLcamera&, const GLobj&)> _cameraRender) {
		_cameraConfig(*_pCamera);
		for (auto pObj : cckit::GLobj::Objs())
			_cameraRender(*_pCamera, *pObj);
	}

	void GLobj::PrepareRenderStates(std::function<void(glm::mat4&)> _outlineModelMatConfig) const {
		glm::vec3 scale = mLocalScale * mScale;

		mPosition = mUnmodifiedPosition;
		mModelMat = glm::mat4();
		// scale => rotate => translate
		glm::mat4 translateMat;
		mTranslateFunc(translateMat);
		mPosition = translateMat * mPosition;
		mModelMat = glm::translate(mModelMat, mPosition);

		glm::mat4 rotateMat;
		mRotateFunc(rotateMat);
		rotateMat = glm::rotate(rotateMat, glm::radians(mRotation.x), glm::vec3(1, 0, 0));
		rotateMat = glm::rotate(rotateMat, glm::radians(mRotation.y), glm::vec3(0, 1, 0));
		rotateMat = glm::rotate(rotateMat, glm::radians(mRotation.z), glm::vec3(0, 0, 1));
		if (mbFacingTarget) {
			mbFacingTarget = false;
			//rotateMat = glm::lookAtRH(glm::vec3(0), mTargetPos - mPosition);
			rotateMat = glm::lookAt(mPosition, mTargetPos, mFacingMode);
		}
		mModelMat *= rotateMat;
		mModelMat = glm::rotate(mModelMat, glm::radians(mlocalRotation.x), glm::vec3(1, 0, 0));
		mModelMat = glm::rotate(mModelMat, glm::radians(mlocalRotation.y), glm::vec3(0, 1, 0));
		mModelMat = glm::rotate(mModelMat, glm::radians(mlocalRotation.z), glm::vec3(0, 0, 1));

		mScaleFunc(mModelMat);
		mModelMat = glm::scale(mModelMat, scale);
		//! scale => rotate => translate
		_outlineModelMatConfig(mOutlineModelMat = mModelMat);
		mOutlineModelMat = glm::translate(mOutlineModelMat, -mLocalCenter);// symmetry
		mModelMat = glm::translate(mModelMat, -mLocalCenter);// symmetry

		mForward = rotateMat * M_FORWARD_AXIS;
		mRight = rotateMat * M_RIGHT_AXIS;
		mUp = rotateMat * M_UP_AXIS;

		// the next line of code is unnecessary because set_position(const glm::mat4&) is not used here
		//mUnmodifiedPosition = glm::inverse(translateMat) * mPosition;
	}

	inline void GLobj::RenderModel(const GLshader& _shader, std::function<void(const GLshader&)> _uniformConfig
		, const GLshader* _pShaderOutline, std::function<void(const GLshader&)> _uniformConfigOutline) const {
		if (model_ptr())
			model_ptr()->render(_shader, _uniformConfig, _pShaderOutline, _uniformConfigOutline, mbOutlined);
	}

	void GLobj::RenderCoordAxes(const GLshader& _shader
		, std::function<void(const GLshader&)> _uniformConfig) const {
		if (mbCoordAxesDrawn) {
			_uniformConfig(_shader);

			glm::vec3 coordAxes3D[] = {
				mPosition, mPosition + mCoordAxisLength * mForward, mPosition
				, mPosition, mPosition + mCoordAxisLength * mRight, mPosition
				, mPosition, mPosition + mCoordAxisLength * mUp, mPosition
			};
			for (int i = length(coordAxes3D) - 1; i >= 0; --i) {
				mCoordAxes1D[6 * i] = coordAxes3D[i].x;
				mCoordAxes1D[6 * i + 1] = coordAxes3D[i].y;
				mCoordAxes1D[6 * i + 2] = coordAxes3D[i].z;
			}
			
			GLuint vaoHandle, vboHandle;
			glGenVertexArrays(1, &vaoHandle);
			glGenBuffers(1, &vboHandle);

			glBindVertexArray(vaoHandle);
			glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
			glBufferData(GL_ARRAY_BUFFER, sizeof(mCoordAxes1D), mCoordAxes1D, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, length(coordAxes3D));

			glDeleteVertexArrays(1, &vaoHandle);
			glDeleteBuffers(1, &vboHandle);
		}
	}
}

#endif // !CCKIT_GLOBJ_H
