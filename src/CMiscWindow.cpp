#include "CMiscWindow.h"

#include <imgui/imgui.h>

#include <Game/IDeveloperInput.h>
#include <Misc/ICommand.h>

void CMiscWindow::Draw()
{
	if (ImGui::Begin("Misc"))
	{
		if (ImGui::CollapsingHeader("Options"))
		{
			bool devInputEnabled = TheDeveloperInput().GetEnabled();

			if (ImGui::Checkbox("DevInput", &devInputEnabled))
			{
				TheDeveloperInput().SetEnabled(devInputEnabled);
			}

			ImGui::Text("NoClip"); ImGui::SameLine();

			if (ImGui::Button("Toggle"))
			{
				Command().ExecuteCommandString("noclip");
			}

			ImGui::Text("LoadPoint"); ImGui::SameLine();

			if (ImGui::Button("Load"))
			{
				Command().AsyncCommand("loadpoint a2_strange1b");
				//Command().AsyncCommand("loadmap act1/heli/heli1");
				//Command().AsyncCommand("loadmapaddteam act1/heli/heli1");
			}
		}

		if (ImGui::CollapsingHeader("Draw"))
		{
			assert(mOverlay);
			
			bool showEntityName = mOverlay->GetShowEntityName();

			if (ImGui::Checkbox("Draw Entity Names", &showEntityName))
			{
				mOverlay->SetShowEntityName(showEntityName);
			}

			bool showEntityPos = mOverlay->GetShowEntityPos();

			if (ImGui::Checkbox("Draw Entity Positions", &showEntityPos))
			{
				mOverlay->SetShowEntityPos(showEntityPos);
			}

			bool showEntityAngle = mOverlay->GetShowEntityAngle();

			if (ImGui::Checkbox("Draw Entity Angles", &showEntityAngle))
			{
				mOverlay->SetShowEntityAngle(showEntityAngle);
			}

			ImVec4 entityInfoColor = mOverlay->GetEntityInfoColor();

			if (ImGui::ColorEdit4("Entity Info Color", &entityInfoColor.x, ImGuiColorEditFlags_NoInputs))
			{
				mOverlay->SetEntityInfoColor(entityInfoColor);
			}

			float entityInfoRange = mOverlay->GetEntityInfoRange();

			if (ImGui::SliderFloat("Entity Info Range", &entityInfoRange, 128.0F, 1024.0F))
			{
				mOverlay->SetEntityInfoRange(entityInfoRange);
			}
		}
	}

	ImGui::End();
}