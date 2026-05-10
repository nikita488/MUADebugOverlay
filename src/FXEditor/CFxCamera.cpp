#include "CFxCamera.h"

#include <imgui/imgui.h>

#include <igGapMath.h>
#include <igMath/igMath.h>

#include "CFxEditor.h"

#define MIN_DISTANCE 25.0F
#define MAX_DISTANCE 2048.0F

void MakeLookAt(const igVec3f& eye, const igVec3f& point, igMatrix44f& mat)
{
	mat.makeLookAt(eye, point, igVec3f::ZVector);
	mat.invert(mat);
}

CFxCamera::CFxCamera()
{
	Reset();
}

void CFxCamera::Reset()
{
	mPitch = 0.0F;
	mRoll = 0.0F;
	mYaw = 0.0F;
	mDistance = 0.0F;
	mPanX = 0.0F;
	mPanY = 0.0F;
	mEye = igVec3f::ZeroVector;
	mTarget = igVec3f::ZeroVector;
	mEyeOffset = igVec3f::ZeroVector;
}

void CFxCamera::Initialize()
{
	//mDistance = 256.0F;
	//mYaw = -90.0F;
	//mPitch = 90.0F;
	//LookAt(igVec3f(-256.0F, 0.0F, 0.0F), igVec3f::ZeroVector);
	LookAt(igVec3f(-160.0F, 0.0F, 0.0F), igVec3f::ZeroVector);
	UpdateCameraMatrix();
}

void CFxCamera::RunFrame()
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
	{
		return;
	}

	float x = io.MouseDelta.x / io.DisplaySize.x;
	float y = io.MouseDelta.y / io.DisplaySize.y;
	
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		if (io.KeyAlt)
		{
			PanCamera(x, y);
		}
		else
		{
			OrbitCamera(x, y);
		}
	}
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		ZoomCamera(y);
	}
}

void CFxCamera::UpdateCameraMatrix()
{
	using namespace Math;
	
	igMatrix44f hpr;
	hpr.makeHPR(mYaw, mPitch, mRoll);

	igVec3f forward, right, up;

	hpr.getRow(2, forward);
	hpr.getRow(0, right);
	hpr.getRow(1, up);

	mTarget = right * mPanX + up * mPanY;
	mDistance = igClamp(mDistance, MIN_DISTANCE, MAX_DISTANCE);
	
	mEye = forward * -mDistance;
	mEye += mTarget;
	mEye += mEyeOffset;

	igMatrix44f cameraMtx;
	MakeLookAt(mEye, mTarget, cameraMtx);

	mCameraMatrix = cameraMtx;
}

void CFxCamera::LookAt(const igVec3f& eye, const igVec3f& target)
{
	MakeLookAt(eye, target, mCameraMatrix);

	mCameraMatrix.getHPR(mYaw, mPitch, mRoll);
	mDistance = (target - eye).length();
}

void CFxCamera::OrbitCamera(float mouseDeltaX, float mouseDeltaY)
{
	using namespace Math;
	
	mYaw -= mouseDeltaX * 360.0F;
	mPitch += mouseDeltaY * 360.0F;

	mYaw = fmodf(mYaw, 360.0F);
	mPitch = igClamp(mPitch, 0.0F, 179.0F);
	UpdateCameraMatrix();
}

void CFxCamera::PanCamera(float mouseDeltaX, float mouseDeltaY)
{
	mPanX += mouseDeltaX * mDistance * 2.0F;
	mPanY += mouseDeltaY * mDistance * 2.0F;
	UpdateCameraMatrix();
}

void CFxCamera::ZoomCamera(float mouseDeltaY)
{
	mDistance += mouseDeltaY * 400.0F;
	UpdateCameraMatrix();
}
