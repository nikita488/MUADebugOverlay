#include "CEarthwormWindow.h"

#include <imgui/imgui.h>

#include <Game/Earthworm.h>

void CEarthwormWindow::Draw()
{
	if (ImGui::Begin("Earthworm"))
	{
		CEarthwormManager& earthwormMgr = TheEarthwormManager();
		
		if (ImGui::TreeNode("Zone Variables"))
		{
			ImGui::Text("%d / %d", earthwormMgr.mZoneVariables.size(), earthwormMgr.mZoneVariables.CAPACITY);

			for (auto it = earthwormMgr.mZoneVariables.begin(); it != earthwormMgr.mZoneVariables.end(); it++)
			{
				ImGui::Text("%s", it.key().c_str());
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Game Variables"))
		{
			ImGui::Text("%d / %d", earthwormMgr.mGameVariables.size(), earthwormMgr.mGameVariables.CAPACITY);

			for (auto it = earthwormMgr.mGameVariables.begin(); it != earthwormMgr.mGameVariables.end(); it++)
			{
				ImGui::Text("%s", it.key().c_str());
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Scripts"))
		{
			ImGui::Text("%d / %d", earthwormMgr.mScriptLookup.size(), earthwormMgr.mScriptLookup.CAPACITY);

			for (auto it = earthwormMgr.mScriptLookup.begin(); it != earthwormMgr.mScriptLookup.end(); it++)
			{
				const CScript* script = it.value();

				if (ImGui::TreeNode(it.key().c_str()))
				{
					ImGui::Text("Local Variables: %d / %d", script->mVariableLookup.size(), script->mVariableLookup.capacity());

					if (ImGui::TreeNode("Local Variables"))
					{
						for (auto it = script->mVariableLookup.begin(); it != script->mVariableLookup.end(); it++)
						{
							ImGui::Text("%s", it.key().c_str());
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}