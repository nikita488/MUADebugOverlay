#include "CCameraWindow.h"

#include <imgui/imgui.h>

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#include <Misc/ICommand.h>
#include <Misc/COrgAngles.h>
#include <Game/ICamera.h>

void ToggleFreeView()
{
	//TheClock().TogglePauseGametime();
	//TheCamera().SetFreeViewAllowed(true);
	//
	//if (TheCamera().GetFreeViewAllowed())
	//{
	//	TheCamera().ToggleFreeView();
	//}
}

void TogglePausedFreeView()
{
	//TheCamera().SetFreeViewPaused(!TheCamera().GetFreeViewPaused());
}

void CCameraWindow::Draw()
{
	if (ImGui::Begin("Camera"))
	{
		TheCamera().SetFreeViewAllowed(true);
		
		bool inFreeView = TheCamera().CameraIsInFreeView();

		if (ImGui::Checkbox("Enable Free Camera", &inFreeView))
		{
			Command().ExecuteCommandString("freeview");
		}

		bool freeViewPaused = TheCamera().GetFreeViewPaused();

		if (ImGui::Checkbox("Allow Paused Free Camera", &freeViewPaused))
		{
			TheCamera().SetFreeViewPaused(freeViewPaused);
		}

		if (ImGui::CollapsingHeader("Info"))
		{
			const igVec3f& viewPoint = TheCamera().GetViewPoint();
			const igVec3f& focusPoint = TheCamera().GetFocusPoint();
			float fov = TheCamera().GetCameraFOV();

			igVec3f angles = ConvertDirectionToAngles(focusPoint - viewPoint);
			float rad2deg = 180.0F / 3.1415927F;
			float roll = rad2deg * angles[0];
			float pitch = rad2deg * angles[1];
			float yaw = rad2deg * angles[2];

			ImGui::Text("Camera Pos: %.2f %.2f %.2f", viewPoint[0], viewPoint[1], viewPoint[2]);
			ImGui::Text("Camera Focus: %.2f %.2f %.2f", focusPoint[0], focusPoint[1], focusPoint[2]);
			ImGui::Text("Roll: %.1f Pitch: %.1f Yaw: %.1f", roll, pitch, yaw);
			ImGui::Text("Camera FOV: %3.1f", fov);

			ratl::string_vs<128> clipboardText;

			if (ImGui::Button("Copy Position"))
			{
				snprintf(clipboardText.c_str(), clipboardText.capacity(), "\" %.3f %.3f %.3f \"", viewPoint[0], viewPoint[1], viewPoint[2]);
				ImGui::SetClipboardText(clipboardText.c_str());
			}

			if (ImGui::Button("Copy Focus"))
			{
				snprintf(clipboardText.c_str(), clipboardText.capacity(), "\" %.3f %.3f %.3f \"", focusPoint[0], focusPoint[1], focusPoint[2]);
				ImGui::SetClipboardText(clipboardText.c_str());
			}

			if (ImGui::Button("Copy Angles"))
			{
				snprintf(clipboardText.c_str(), clipboardText.capacity(), "\" %.3f %.3f %.3f \"", roll, pitch, yaw);
				ImGui::SetClipboardText(clipboardText.c_str());
			}

			if (ImGui::Button("Copy FOV"))
			{
				snprintf(clipboardText.c_str(), clipboardText.capacity(), "%.3f", fov);
				ImGui::SetClipboardText(clipboardText.c_str());
			}
		}
	}

	ImGui::End();
}
