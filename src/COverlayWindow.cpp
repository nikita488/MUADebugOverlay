#include "COverlayWindow.h"

#include <Input/IInput.h>

#include <Game/IGame.h>
#include <Game/IQuery.h>
#include <Game/CMarvelCamera.h>
#include <Game/CActor.h>

#include <Misc/Common.h>
#include <Misc/CMemory.h>

#include <Display/IDisplay.h>


#include <igGapCore.h>
#include <igCore/igStringObj.h>

#include <igGapMath.h>
#include <igMath/igVec3f.h>

COverlayWindow::COverlayWindow() : 
	mEntityInfoColor(1.0F, 1.0F, 0.0F, 1.0F), 
	mEntityInfoRange(150.0F), 
	mShowEntityName(false), 
	mShowEntityPos(false), 
	mShowEntityAngle(false)
{
}

void COverlayWindow::Draw()
{
	ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings);

	const CEntityID& leader = CActor::GroupLeader(TheInput().PrimaryPlayerId());
	
	if (leader.IsValid())
	{
		CActor* actor = entity_cast<CActor>(leader.Ptr());
	
		if (actor && (mShowEntityName || mShowEntityPos || mShowEntityAngle))
		{
			CQueryH handle(CEntity::mFactory, CEntityID(0));
	
			handle->FilterRadiusTouch(actor->Origin(), mEntityInfoRange, 0.0F, false);
	
			COrder<CEntity> order(handle, CSortOrder());
	
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImU32 color = ImGui::GetColorU32(mEntityInfoColor);
	
			for (int i = 0; i < order.Size(); i++)
			{
				CEntity* ent = order[i];
	
				const igVec3f& origin = ent->Origin();
				const igMatrix44f& projMat = reinterpret_cast<CMarvelCamera*>(&TheCamera())->mProjectionView16x9;
	
				igVec3f result;
				result.transformPoint(origin, projMat);
	
				float x = TheDisplay().GetWidth() * ((1.0F + result[0]) / 2.0F);
				float y = TheDisplay().GetHeight() * ((1.0F - result[1]) / 2.0F);
	
				igStringObjRef label = NewInstanceRef<igStringObj>(kIGMemoryPoolTemporary);
				bool addNewLine = false;
	
				if (mShowEntityName)
				{
					label->append(ent->mName.c_str());
					addNewLine = true;
				}
	
				if (mShowEntityPos)
				{
					if (addNewLine)
					{
						label->append("\n");
					}
	
					char buf[128];
					sprintf(buf, "%.02f %.02f %.02f", origin[0], origin[1], origin[2]);
					label->append(buf);
					addNewLine = true;
				}
	
				if (mShowEntityAngle)
				{
					if (addNewLine)
					{
						label->append("\n");
					}
	
					const Math::igVec3f& angles = ent->Angles();
	
					char buf[128];
					sprintf(buf, "%.02f %.02f %.02f", angles[0], angles[1], angles[2]);
					label->append(buf);
				}
	
				ImVec2& size = ImGui::CalcTextSize(label->getBuffer());
				ImGui::GetWindowDrawList()->AddText(ImVec2(x - (size.x / 2.0F), y - (size.y / 2.0F)), color, label->getBuffer());//0xAABBGGRR
				label = NULL;
			}
	
			ImGui::PopFont();
		}
	}

	ImGui::End();
}