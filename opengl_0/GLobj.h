#ifndef CCKIT_GLOBJ_H
#define CCKIT_GLOBJ_H

#include <cfloat>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLmodel.h"
#include "GLlight.h"

#define FORWARD_AXIS glm::vec3(0, 0, 1)
#define RIGHT_AXIS glm::vec3(-1, 0, 0)
#define UP_AXIS glm::vec3(0, 1, 0)

#define FORWARD_AXIS_COLOR  glm::vec3(1, 0, 0)
#define RIGHT_AXIS_COLOR  glm::vec3(0, 1, 0)
#define UP_AXIS_COLOR  glm::vec3(0, 0, 1)

namespace cckit
{
	class GLcamera;

	class GLobj
	{
	private:
		static glm::vec3 M_FORWARD_AXIS;
		static glm::vec3 M_RIGHT_AXIS;
		static glm::vec3 M_UP_AXIS;
	public:
		GLobj();
		explicit GLobj(const std::string& _modelPath);

		void setup_render_config() const;

		const glm::mat4& model_mat() const { return mModelMat; }
		GLmodel* model_ptr() const { return mpModel; }

		const glm::vec3& position() const { return mPosition; }
		void set_position(const glm::vec3& _position) {
			mPosition = mUnmodifiedPosition = _position;
		}

	public:
		glm::vec3 mLocalPosition;
		glm::vec3 mlocalRotation;
		glm::vec3 mLocalScale;
		glm::vec3 mRotation;
		glm::vec3 mScale;

		mutable glm::vec3 mForward;
		mutable glm::vec3 mRight;
		mutable glm::vec3 mUp;

		GLfloat mCoordAxisLength;
		bool mbCoordAxesDrawn;
		glm::vec3 mAxisColors[3];// forward, right, up

		std::function<void(glm::mat4&)> mTranslateFunc;
		std::function<void(glm::mat4&)> mRotateFunc;
		std::function<void(glm::mat4&)> mScaleFunc;
	private:
		mutable glm::vec3 mPosition;
		mutable glm::vec3 mUnmodifiedPosition;
	private:
		GLmodel* mpModel;
		glm::vec3 mLocalCenter;

		mutable glm::mat4 mModelMat;
		mutable GLfloat mCoordAxes1D[54];// 6:3:3 <=> vertAttribs : verts : coordAxes

		friend GLcamera;
	};
	glm::vec3 GLobj::M_FORWARD_AXIS = FORWARD_AXIS;
	glm::vec3 GLobj::M_RIGHT_AXIS = RIGHT_AXIS;
	glm::vec3 GLobj::M_UP_AXIS = UP_AXIS;

	inline GLobj::GLobj()
		: mpModel(nullptr), mModelMat()
		, mLocalPosition(0.0f), mlocalRotation(0.0f), mLocalScale(1.0f)
		, mPosition(0.0f), mRotation(0.0f), mScale(1.0f)
		, mForward(M_FORWARD_AXIS[0], M_FORWARD_AXIS[1], M_FORWARD_AXIS[2])
		, mRight(M_RIGHT_AXIS[0], M_RIGHT_AXIS[1], M_RIGHT_AXIS[2])
		, mUp(M_UP_AXIS[0], M_UP_AXIS[1], M_UP_AXIS[2])
		, mCoordAxisLength(1.0f), mbCoordAxesDrawn(false)
		, mTranslateFunc([](glm::mat4&) {}), mRotateFunc([](glm::mat4&) {}), mScaleFunc([](glm::mat4&) {})
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
	}

	GLobj::GLobj(const std::string& _modelPath)
		: GLobj() {
		mpModel = new GLmodel(_modelPath);
		if (!mpModel) return;

		GLfloat xMin = FLT_MAX, xMax = FLT_MIN
			, yMin = FLT_MAX, yMax = FLT_MIN
			, zMin = FLT_MAX, zMax = FLT_MIN;

		for (GLmesh* pMesh : mpModel->mMeshes) {
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
	}

	void GLobj::setup_render_config() const {
		glm::vec3 scale = mLocalScale * mScale;

		mPosition = mUnmodifiedPosition;
		mModelMat = glm::mat4();

		glm::mat4 translateMat;
		mTranslateFunc(translateMat);
		mPosition = translateMat * mPosition;
		mModelMat = glm::translate(mModelMat, mPosition);
		glm::mat4 rotateMat;
		mRotateFunc(rotateMat);
		rotateMat = glm::rotate(rotateMat, glm::radians(mRotation.x), glm::vec3(1, 0, 0));
		rotateMat = glm::rotate(rotateMat, glm::radians(mRotation.y), glm::vec3(0, 1, 0));
		rotateMat = glm::rotate(rotateMat, glm::radians(mRotation.z), glm::vec3(0, 0, 1));
		mModelMat *= rotateMat;
		mModelMat = glm::rotate(mModelMat, glm::radians(mlocalRotation.x), glm::vec3(1, 0, 0));
		mModelMat = glm::rotate(mModelMat, glm::radians(mlocalRotation.y), glm::vec3(0, 1, 0));
		mModelMat = glm::rotate(mModelMat, glm::radians(mlocalRotation.z), glm::vec3(0, 0, 1));
		mScaleFunc(mModelMat);
		mModelMat = glm::scale(mModelMat, scale);

		mModelMat = glm::translate(mModelMat, -mLocalCenter);

		mForward = rotateMat * M_FORWARD_AXIS;
		mRight = rotateMat * M_RIGHT_AXIS;
		mUp = rotateMat * M_UP_AXIS;
		
		// the next line of code is unnecessary because set_position(const glm::mat4&) is not used here
		//mUnmodifiedPosition = glm::inverse(translateMat) * mPosition;
	}
}

#endif // !CCKIT_GLOBJ_H
