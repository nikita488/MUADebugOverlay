#include "CMiscWindow.h"

#include <imgui/imgui.h>

#include <Common/Ratl/string_vs.h>

#include <Game/IDeveloperInput.h>
#include <Misc/ICommand.h>

#include <UI/IMenuSystem.h>

#include "FXEditor/Segments/PropertyGroups/FxImGuiWidgets.h"

CMiscWindow::CMiscWindow() :
	mOverlay(NULL),
	mTeamRepAmount(1),
	mXPAmount(1),
	mXPAllAmount(1),
	mMoneyAmount(1)
{
}

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

		if (ImGui::CollapsingHeader("Commands"))
		{
			ImGui::Indent();
			
			if (ImGui::CollapsingHeader("Menu"))
			{
				GroupSection group(TheMenuMgr().IsActive());

				if (ImGui::Button("Menu : Debug"))
				{
					Command().AsyncCommand("openmenu debug");
				}

				if (ImGui::Button("Menu : Codex"))
				{
					Command().AsyncCommand("openmenu codex");
				}

				if (ImGui::Button("Menu : Fx Editor"))
				{
					Command().AsyncCommand("openmenu fxeditor");
				}
			}
			
			if (ImGui::CollapsingHeader("Give"))
			{
				{
					if (ImGui::Button("Give##TeamReputationButton"))
					{
						ratl::string_vs<32> cmd;
						sprintf_s(cmd.c_str(), cmd.capacity(), "give rep %d", mTeamRepAmount);
						Command().ExecuteCommandString(cmd.c_str());
					}

					ImGui::SameLine();
					ImGui::DragInt("##TeamReputationAmount", &mTeamRepAmount, 1.0F, 1, 2147483647, "+%d Team reputation");
				}

				{
					if (ImGui::Button("Give##XpButton"))
					{
						ratl::string_vs<32> cmd;
						sprintf_s(cmd.c_str(), cmd.capacity(), "give xp %d", mXPAmount);
						Command().ExecuteCommandString(cmd.c_str());
					}

					ImGui::SameLine();
					ImGui::DragInt("##XpAmount", &mXPAmount, 1.0F, 1, 2147483647, "+%d XP to Current heroes");
				}

				{
					if (ImGui::Button("Give##XpAllButton"))
					{
						ratl::string_vs<32> cmd;
						sprintf_s(cmd.c_str(), cmd.capacity(), "give xpall %d", mXPAllAmount);
						Command().ExecuteCommandString(cmd.c_str());
					}

					ImGui::SameLine();
					ImGui::DragInt("##XpAllAmount", &mXPAllAmount, 1.0F, 1, 2147483647, "+%d XP to All heroes");
				}

				{
					if (ImGui::Button("Give##MoneyButton"))
					{
						ratl::string_vs<32> cmd;
						sprintf_s(cmd.c_str(), cmd.capacity(), "give money %d", mMoneyAmount);
						Command().ExecuteCommandString(cmd.c_str());
					}

					ImGui::SameLine();
					ImGui::DragInt("##MoneyAmount", &mMoneyAmount, 1.0F, 1, 2147483647, "+%d Money to Current hero");
				}
			}

			if (ImGui::CollapsingHeader("Player"))
			{
				if (ImGui::Button("Toggle NoClip"))
				{
					Command().ExecuteCommandString("noclip");
				}

				if (ImGui::Button("Fill Xtreme bar"))
				{
					Command().ExecuteCommandString("give xtreme");
				}

				if (ImGui::Button("Resurrect heroes"))
				{
					Command().ExecuteCommandString("give res");
				}
			}

			ImGui::Unindent();
		}
	}

	ImGui::End();
}