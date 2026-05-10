#include "CFxPlayback.h"

#include <cmath>
#include <imgui/imgui.h>

#include <FX/CFxTemplate.h>

#include "IFxEditor.h"
#include "Segments/PropertyGroups/FxImGuiWidgets.h"

void CFxPlayback::RepeatModeGroup()
{
	bool lastPersistent = mRepeatMode == REPEAT_PERSISTENT;
	CFxFile& file = TheFxEditor().GetFile();
	
	ImGui::SeparatorText("Repeat Mode");

	bool changed = false;

	changed |= ImGui::RadioButton("Play Once", &mRepeatMode, REPEAT_ONCE);
	changed |= ImGui::RadioButton("Repeat until stopped", &mRepeatMode, REPEAT_UNTIL_STOPPED);
	changed |= ImGui::RadioButton("Repeat for", &mRepeatMode, REPEAT_FOR_DURATION);

	{
		GroupSection section(mRepeatMode == REPEAT_FOR_DURATION);

		ImGui::SameLine();
		ImGui::DragFloat("##PlayDuration", &mPlayDuration, 1.0F, 0.0F, 100000000.0F, "%g seconds");
	}

	changed |= ImGui::RadioButton("Persistent", &mRepeatMode, REPEAT_PERSISTENT);

	if (changed && file.IsValid())
	{
		bool isPersistent = mRepeatMode == REPEAT_PERSISTENT;
		
		if (isPersistent != lastPersistent)
		{
			file.GetTemplate()->field_12 = isPersistent;

			for (int i = 0; i < file.GetSegmentCount(); i++)
			{
				IFxSegment* segment = file.GetSegment(i);
				segment->SetPrimitiveFlag(0x2, isPersistent);
			}
		}
	}
}

void CFxPlayback::RepeatRateGroup()
{
	CFxFile& file = TheFxEditor().GetFile();
	
	ImGui::SeparatorText("Repeat Rate");
	ImGui::Checkbox("Respawn effect every frame", &mRespawnEveryFrame);

	{
		GroupSection section(!mRespawnEveryFrame);
		float minRate = 0.05F;
		float maxRate = 60.0F;
		
		if (ImGui::DragFloat("Repeat rate", &mRepeatRate, 0.1F, minRate, maxRate, "%g", ImGuiSliderFlags_AlwaysClamp))
		{
			if (file.IsValid())
			{
				file.GetTemplate()->field_4 = mRepeatRate;
			}
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Seconds per effects spawn");
		}

		float repeatFrequency = 1.0F / mRepeatRate;
		float minFrequency = 1.0F / maxRate;
		float maxFrequency = 1.0F / minRate;

		if (ImGui::DragFloat("Repeat frequency", &repeatFrequency, 1.0F, minFrequency, maxFrequency, "%g", ImGuiSliderFlags_AlwaysClamp))
		{
			mRepeatRate = 1.0F / repeatFrequency;

			if (file.IsValid())
			{
				file.GetTemplate()->field_4 = mRepeatRate;
			}
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Effect spawns per second");
		}

		if (ImGui::DragFloat("Random Time", &mRandomTime, 0.1F, 0.0F, 0.0F, "%g", ImGuiSliderFlags_AlwaysClamp))
		{
			if (file.IsValid())
			{
				file.GetTemplate()->field_8 = mRandomTime;
			}
		}

		if (mRepeatMode == REPEAT_FOR_DURATION && !mRespawnEveryFrame)
		{
			int totalRepetitions = int(std::ceil(mPlayDuration * repeatFrequency));

			ImGui::Text("Total repetitions: %d", totalRepetitions);
		}
		else
		{
			ImGui::Text("Total repetitions: n/a");
		}
	}
}

void CFxPlayback::SpawnPointMovementGroup()
{
	ImGui::SeparatorText("Spawn Point Movement");
	ImGui::Checkbox("Animate effect spawn location", &mAnimateSpawnPoint);

	{
		GroupSection section(mAnimateSpawnPoint);
		
		ImGui::DragFloat3("Velocity", mSpawnPointVelocity.vec, 1.0F, 0.0F, 0.0F, "%g");
		ImGui::DragFloat("Reset location time##ResetLocationTime", &mSpawnPointResetTime, 1.0F, 0.0F, 100000000.0F, "%g seconds");
	}
}

CFxPlayback::CFxPlayback()
{
	Reset();
}

void CFxPlayback::Initialize(CFxTemplate* fxTemplate)
{
	assert(fxTemplate);

	mRepeatRate = fxTemplate->field_4;
	mRandomTime = fxTemplate->field_8;

	if (fxTemplate->field_12)
	{
		mRepeatMode = REPEAT_PERSISTENT;
	}
	else if (mRepeatRate > 0.0F || mRandomTime > 0.0F)
	{
		mRepeatMode = REPEAT_UNTIL_STOPPED;
	}
	else
	{
		mRepeatMode = REPEAT_ONCE;
	}
}

void CFxPlayback::Reset()
{
	mVisible = false;
	mRepeatMode = REPEAT_ONCE;
	mPlayDuration = 2.0F;
	mRespawnEveryFrame = false;
	mRepeatRate = 0.3F;
	mRandomTime = 0.0F;
	mAnimateSpawnPoint = false;
	mSpawnPointVelocity = igVec3f::ZeroVector;
	mSpawnPointResetTime = 1.0F;
}

void CFxPlayback::Update()
{	
	if (!mVisible)
	{
		return;
	}
	
	if (ImGui::Begin("Playback Settings", &mVisible, ImGuiWindowFlags_AlwaysAutoResize))
	{
		RepeatModeGroup();

		{
			GroupSection section(mRepeatMode == REPEAT_UNTIL_STOPPED || mRepeatMode == REPEAT_FOR_DURATION);
			
			RepeatRateGroup();
			SpawnPointMovementGroup();
		}
	}

	ImGui::End();
}
