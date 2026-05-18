#include "CCameraWindow.h"

#include <imgui/imgui.h>

#include <Misc/ICommand.h>
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
	}

	ImGui::End();
}
