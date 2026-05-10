#include "CFxFlagsGroup.h"

#include <imgui/imgui.h>

const char* CFxFlagsGroup::GetName() const
{
	return "Flags";
}

void CFxFlagsGroup::Initialize(IFxSegment* segment)
{
}

bool FlagsTable(const char* label, unsigned int& flags)
{
	bool valueChanged = false;

	ImGui::PushID(label);
	ImGui::SeparatorText(label);
	valueChanged |= ImGui::InputScalar("##flagsInput", ImGuiDataType_U32, &flags, NULL, NULL, "%u");

	if (ImGui::BeginTable("flagsSplit", 4))
	{
		for (unsigned int i = 0; i < 32; i++)
		{
			char label[64];
			snprintf(label, 64, "%#x", 1 << i);

			ImGui::TableNextColumn();
			valueChanged |= ImGui::CheckboxFlags(label, &flags, 1 << i);
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
	return valueChanged;
}

bool CFxFlagsGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	bool valueChanged = false;
	
	valueChanged |= FlagsTable("Spawn Flags", primTemplate->field_56);
	valueChanged |= FlagsTable("Primitive Flags", primTemplate->field_48);
	valueChanged |= FlagsTable("Primitive Flags 2", primTemplate->field_52);
	return valueChanged;
}