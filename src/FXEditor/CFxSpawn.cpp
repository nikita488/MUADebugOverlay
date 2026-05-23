#include "CFxSpawn.h"

#include <imgui/imgui.h>

#include "Segments/IFxSegment.h"
#include "Segments/PropertyGroups/FxImGuiWidgets.h"

float CFxSpawn::NO_LIFE_OVERRIDE = -1.0F;
igVec3f CFxSpawn::NO_COLOR_OVERRIDE(-1.0F, -1.0F, -1.0F);

CFxSpawn::CFxSpawn()
{
	Reset();
}

void CFxSpawn::Reset()
{
	mVisible = false;
	mOrigin = igVec3f::ZeroVector;
	mForwardAxis = igVec3f::ZeroVector;
	mLevel = 0;
	mLife = 0.0F;
	mTimeScale = 1.0F;
	mSizeScale = 1.0F;
	mColor = igVec3f::ZeroVector;
	mOverrideLife = false;
	mOverrideColor = false;
}

void CFxSpawn::Update()
{
	if (!mVisible)
	{
		return;
	}

	if (ImGui::Begin("Spawn Settings", &mVisible, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::DragFloat3("Fx Origin", mOrigin.vec, 1.0F, 0.0F, 0.0F, "%g");
		ImGui::DragFloat3("Fx Forward Axis", mForwardAxis.vec, 1.0F, 0.0F, 0.0F, "%g");
		
		if (ImGui::InputInt("Fx Level", &mLevel))
		{
			mLevel = std::clamp<int>(mLevel, LEVEL_ALL, LEVEL_8);
		}

		ImGui::Separator();

		ImGui::DragFloat("Fx Time Scale", &mTimeScale, 1.0F, 0.0F, 0.0F, "%g");
		ImGui::DragFloat("Fx Size Scale", &mSizeScale, 1.0F, 0.0F, 0.0F, "%g");

		ImGui::Separator();

		ImGui::Checkbox("Override Fx Life", &mOverrideLife); ImGui::SameLine();

		{
			GroupSection section(mOverrideLife);
			ImGui::DragFloat("##FxLife", &mLife, 1.0F, 0.0F, 1000000.0F, "%g");
		}

		ImGui::Checkbox("Override Fx Color", &mOverrideColor); ImGui::SameLine();

		{
			GroupSection section(mOverrideColor);
			ImGui::ColorEdit3("##FxColor", mColor.vec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
		}

		ImGui::Separator();

		if (ImGui::Button("Reset to default"))
		{
			Reset();
			mVisible = true;
		}
	}

	ImGui::End();
}
