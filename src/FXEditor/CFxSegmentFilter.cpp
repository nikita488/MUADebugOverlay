#include "CFxSegmentFilter.h"

#include <algorithm>
#include <imgui/imgui.h>

#include <Misc/Common.h>

CFxSegmentFilter::CFxSegmentFilter()
{
	Reset();
}

void CFxSegmentFilter::Reset()
{
	mPlatform = PLATFORM_ALL;
	mFxLevel = LEVEL_ALL;
}

bool CFxSegmentFilter::Matches(IFxSegment* segment)
{
	return segment->IsPlatformSupported(mPlatform) && 
		segment->IsFxLevelSupported(mFxLevel);
}

static IDTable platformTable[PLATFORM_COUNT + 1]
{
	{ "All", PLATFORM_ALL },
	{ "PlayStation 2/PC (Low)", PLATFORM_PS2 },
	{ "PlayStation 3", PLATFORM_PS3 },
	{ "PSP", PLATFORM_PSP },
	{ "Wii", PLATFORM_WII },
	{ "XBOX", PLATFORM_XBOX },
	{ "Xbox 360/PC (High)", PLATFORM_XENON },
};

bool PlatformCombo(EFxPlatform& platform)
{
	bool changed = false;
	int previewItem = 0;

	for (int i = 0; i < PLATFORM_COUNT + 1; i++)
	{
		if (platformTable[i].id == platform)
		{
			previewItem = i;
			break;
		}
	}

	if (ImGui::BeginCombo("Platform", platformTable[previewItem].name))
	{
		for (int i = 0; i < PLATFORM_COUNT + 1; i++)
		{
			const IDTable& item = platformTable[i];
			bool selected = item.id == platform;

			if (ImGui::Selectable(item.name, selected))
			{
				platform = EFxPlatform(item.id);
				changed = true;
			}

			if (selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	return changed;
}

bool CFxSegmentFilter::Update()
{
	bool changed = false;
	
	changed |= PlatformCombo(mPlatform);

	if (ImGui::InputInt("Fx Level", (int*)&mFxLevel))
	{
		mFxLevel = std::clamp(mFxLevel, LEVEL_ALL, LEVEL_8);
		changed = true;
	}

	return changed;
}
