#pragma once

#include <igGapMath.h>
#include <igMath/igVec2f.h>
#include <igMath/igVec3f.h>
#include <igMath/igMatrix44f.h>

using namespace Gap;

class CFxCamera
{
private:
	igMatrix44f mCameraMatrix;

	float mPitch;
	float mRoll;
	float mYaw;

	float mDistance;

	float mPanX;
	float mPanY;

	igVec3f mEye;
	igVec3f mTarget;
	igVec3f mEyeOffset;

private:
	void OrbitCamera(float mouseDeltaX, float mouseDeltaY);
	void PanCamera(float mouseDeltaX, float mouseDeltaY);
	void ZoomCamera(float mouseDeltaY);
	void UpdateCameraMatrix();

public:
	CFxCamera();

	void Reset();

	void Initialize();

	void RunFrame();

	void LookAt(const igVec3f& eye, const igVec3f& target);

	inline const igMatrix44f& GetCameraMatrix() const
	{
		return mCameraMatrix;
	}
};

