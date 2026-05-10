#include "CFxGenerationGroup.h"

#include <imgui/imgui.h>

#include <Misc/Common.h>
#include <Misc/ResourceSpecifier.h>

#include <Fx/CFxTemplateManager.h>

#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

#include "../../IFxEditor.h"

CFxGenerationGroup::CFxGenerationGroup() : 
	mIntervalMode(0)
{
}

const char* CFxGenerationGroup::GetName() const
{
	return "Generation";
}

void CFxGenerationGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	unsigned int& flags = primTemplate->field_48;

	mName = segment->GetName();
	mDelay.Set(primTemplate->field_8);
	mCount.Set(primTemplate->field_16);
	mLife.Set(primTemplate->field_24);
	mDeathFxFile = segment->GetDeathFxFile();
	mIntervalFxFile = segment->GetIntervalFxFile();

	mIntervalMode = (flags & 0x4000) != 0 ? 1 : 0;
}

bool CFxGenerationGroup::PlatformOverrides(IFxSegment* segment)
{
	static const char* platforms[PLATFORM_COUNT] =
	{
		"Wii",
		"PS2/PC (Low)",
		"Xbox",
		"X360/PC (High)",
		"PSP",
		"PS3"
	};

	bool valueChanged = false;

	ImGui::SeparatorText("Platform Overrides");

	bool usePlatformOverrides = segment->GetUsePlatformOverrides();

	if (ImGui::Checkbox("Enable platform overrides", &usePlatformOverrides))
	{
		segment->SetUsePlatformOverrides(usePlatformOverrides);
		valueChanged = true;
	}

	{
		GroupSection section(usePlatformOverrides);

		if (ImGui::BeginTable("PlatformTable", 2))
		{
			for (int i = PLATFORM_FIRST; i < PLATFORM_LAST; i++)
			{
				EFxPlatform platform = EFxPlatform(i);
				bool supported = segment->IsPlatformSupported(platform);

				ImGui::TableNextColumn();

				if (ImGui::Checkbox(platforms[i], &supported))
				{
					segment->SetPlatformSupported(platform, supported);
					valueChanged = true;
				}
			}

			ImGui::EndTable();
		}
	}

	return valueChanged;
}

bool CFxGenerationGroup::FxLevels(IFxSegment* segment)
{
	static const char* fxLevels[LEVEL_COUNT] =
	{
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
	};

	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	bool valueChanged = false;

	ImGui::SeparatorText("FX Levels");

	if (ImGui::BeginTable("FXLevelsTable", 4))
	{
		for (int i = LEVEL_FIRST; i < LEVEL_LAST; i++)
		{
			EFxLevel level = EFxLevel(i);
			bool supported = segment->IsFxLevelSupported(level);

			ImGui::TableNextColumn();

			if (ImGui::Checkbox(fxLevels[i - 1], &supported))
			{
				segment->SetFxLevelSupported(level, supported);
				valueChanged = true;
			}
		}

		ImGui::EndTable();
	}

	return valueChanged;
}

bool CFxGenerationGroup::PlayOptions(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& spawnFlags = primTemplate->field_56;
	unsigned int& flags = primTemplate->field_48;
	unsigned int& flags2 = primTemplate->field_52;
	
	bool valueChanged = false;
	
	ImGui::SeparatorText("Play Options");

	valueChanged |= ImGui::CheckboxFlags("Spawn at camera position", &spawnFlags, 0x10000000);
	valueChanged |= ImGui::CheckboxFlags("Kill immediately", &flags, 0x10000);
	//valueChanged |= ImGui::CheckboxFlags("Allow lifetime override", &spawnFlags, 0x40000);
	//valueChanged |= CheckboxFlagsInverse("Allow time scaling", &flags2, 0x40);
	//valueChanged |= ImGui::CheckboxFlags("Allow property scaling", &flags2, 0x80);
	valueChanged |= ImGui::CheckboxFlags("Allow life control", &spawnFlags, 0x40000);
	valueChanged |= CheckboxFlagsInverse("Allow time control", &flags2, 0x40);
	valueChanged |= ImGui::CheckboxFlags("Allow scale control", &flags2, 0x80);

	{
		GroupSection section(IsColorSupported(primType));
		//valueChanged |= ImGui::CheckboxFlags("Allow color override", &flags2, 0x4000000);
		valueChanged |= ImGui::CheckboxFlags("Allow color control", &flags2, 0x4000000);
	}

	return valueChanged;
}

bool CFxGenerationGroup::DeathEffect(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& flags = primTemplate->field_48;

	bool valueChanged = false;
	GroupSection sectionA(primType != FXPRIM_TYPE_CAMERA_SHAKE);

	ImGui::SeparatorText("Death Effect");

	bool deathFxEnabled = (flags & 0x1000) != 0;

	valueChanged |= ImGui::CheckboxFlags("Enable Death Effect", &flags, 0x1000);

	IAssetCache& fxCache = TheFxEditor().FxCache();

	{
		GroupSection sectionB(deathFxEnabled);
		ImGuiFileDialogContext ctx = EffectFileDialog("DeathFxFileDlg");

		ctx.SetInitialPath(segment->GetDeathFxFile(), "effects/");

		if (InputAsset("##DeathFxFile", mDeathFxFile.c_str(), mDeathFxFile.capacity(), fxCache, segment->GetDeathFxFile(), ctx))
		{
			segment->SetDeathFxFile(mDeathFxFile.c_str());
			primTemplate->field_60 = TheFxTemplateManager().Find(mDeathFxFile.c_str(), MP_MENU);
			valueChanged = true;
		}
	}

	return valueChanged;
}

bool CFxGenerationGroup::IntervalEffect(IFxSegment* segment, bool& flagsChanged)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& flags = primTemplate->field_48;
	
	bool valueChanged = false;
	GroupSection sectionA(primType != FXPRIM_TYPE_CAMERA_SHAKE);

	ImGui::SeparatorText("Interval Effect");

	bool intervalFxEnabled = (flags & 0x2000) != 0;

	flagsChanged |= ImGui::CheckboxFlags("Enable Interval Effect", &flags, 0x2000);

	{
		GroupSection sectionB(intervalFxEnabled);

		flagsChanged |= ImGui::RadioButton("Time", &mIntervalMode, 0); ImGui::SameLine();
		flagsChanged |= ImGui::RadioButton("Distance", &mIntervalMode, 1);

		valueChanged |= ImGui::DragFloat("Interval", &primTemplate->field_68, 1.0F, 0.0F, 0.0F, "%g");

		IAssetCache& fxCache = TheFxEditor().FxCache();
		ImGuiFileDialogContext ctx = EffectFileDialog("IntervalFxFileDlg");

		ctx.SetInitialPath(segment->GetIntervalFxFile(), "effects/");

		if (InputAsset("##IntervalFxFile", mIntervalFxFile.c_str(), mIntervalFxFile.capacity(), fxCache, segment->GetIntervalFxFile(), ctx))
		{
			segment->SetIntervalFxFile(mIntervalFxFile.c_str());
			primTemplate->field_64 = TheFxTemplateManager().Find(mIntervalFxFile.c_str(), MP_MENU);
			valueChanged = true;
		}
	}

	return valueChanged;
}

bool CFxGenerationGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& spawnFlags = primTemplate->field_56;
	unsigned int& flags = primTemplate->field_48;
	unsigned int& flags2 = primTemplate->field_52;

	bool valueChanged = false;
	bool flagsChanged = false;

	if (ImGui::InputText("Name", mName.c_str(), mName.capacity()))
	{
		segment->SetName(mName.c_str());
		valueChanged = true;
	}

	ImGui::Separator();

	if (FloatGroup("Delay", primTemplate->field_8, mDelay))
	{
		TheFxEditor().UpdateTable();
		valueChanged = true;
	}

	{
		GroupSection section(primType != FXPRIM_TYPE_CAMERA_SHAKE);
		valueChanged |= ImGui::CheckboxFlags("Use even delay distribution", &spawnFlags, 0x10000);
		
		if (FloatGroup("Count", primTemplate->field_16, mCount))
		{
			TheFxEditor().UpdateTable();
			valueChanged = true;
		}
	}

	valueChanged |= FloatGroup("Life", primTemplate->field_24, mLife);

	valueChanged |= PlayOptions(segment);
	valueChanged |= DeathEffect(segment);
	valueChanged |= IntervalEffect(segment, flagsChanged);
	valueChanged |= PlatformOverrides(segment);
	valueChanged |= FxLevels(segment);

	if (flagsChanged)
	{
		if (mIntervalMode == 1)
		{
			flags |= 0x4000;
		}
		else
		{
			flags &= ~0x4000;
		}
	}

	valueChanged |= flagsChanged;

	return valueChanged;
}
