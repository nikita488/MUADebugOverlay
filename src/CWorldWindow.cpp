#include "CWorldWindow.h"

#include <imgui/imgui.h>

#include <Game/CWorld.h>
#include <Game/CSmartEnt.h>

#include <igGapMath.h>
#include <igMath/igVec3f.h>

void CWorldWindow::Draw()
{
	if (ImGui::Begin("World"))
	{
		if (ImGui::CollapsingHeader("Entities"))
		{
			CWorld& world = reinterpret_cast<CWorld&>(TheWorld());
			
			ImGui::Text("%d / %d", world.mEnts.size(), world.mEnts.capacity());

			int index = 0;

			for (auto it = world.mEnts.begin(); it != world.mEnts.end(); it++)
			{
				CEntity* ent = (*it);

				char buf[256];
				sprintf(buf, "%02d: %s (%s)", index + 1, ent->mName.c_str(), ent->mEntDefName.empty() ? "None" : ent->mEntDefName.c_str());

				if (ImGui::TreeNode(buf))
				{
					const igVec3f& origin = ent->Origin();
					const igVec3f& angles = ent->Angles();
					const SAABBox& extent = ent->Extent();
					
					ImGui::Text("Position: %f %f %f", origin[0], origin[1], origin[2]);
					ImGui::Text("Angles: %f %f %f", angles[0], angles[1], angles[2]);
					ImGui::Text("Extent:  %f %f %f -> %f %f %f", extent.mins[0], extent.mins[1], extent.mins[2], extent.maxs[0], extent.maxs[1], extent.maxs[2]);

					if (ent->CollideExtent())
					{
						const SAABBox& collideExtent = *ent->CollideExtent();
						ImGui::Text("Collide Extent: %f %f %f -> %f %f %f", collideExtent.mins[0], collideExtent.mins[1], collideExtent.mins[2], collideExtent.maxs[0], collideExtent.maxs[1], collideExtent.maxs[2]);
					}

					ImGui::TreePop();
				}

				index++;
			}
		}

		if (ImGui::CollapsingHeader("Smart Entities"))
		{
			CSmartEntMgr& smartEntMgr = reinterpret_cast<CSmartEntMgr&>(TheSmartEntMgr());

			ImGui::Text("%d / %d", smartEntMgr.mSmartEnts.size(), smartEntMgr.mSmartEnts.capacity());

			int index = 0;

			for (auto it = smartEntMgr.mSmartEnts.begin(); it != smartEntMgr.mSmartEnts.end(); it++)
			{
				CEntitySave& ent = (*it);

				ImGui::Text("%02d: %s (%s)", index + 1, ent.mEntName.c_str(), ent.mEntName.empty() ? "None" : ent.mEntName.c_str());
				index++;
			}
		}
	}

	ImGui::End();
}