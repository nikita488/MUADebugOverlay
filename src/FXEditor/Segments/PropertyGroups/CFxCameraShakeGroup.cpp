#include "CFxCameraShakeGroup.h"

#include <algorithm>
#include <imgui/imgui.h>

#define MAX_SHAKE_TYPES 4

const char* CFxCameraShakeGroup::GetName() const
{
	return "CameraShake";
}

void CFxCameraShakeGroup::Initialize(IFxSegment* segment)
{
}

bool CFxCameraShakeGroup::Draw(IFxSegment* segment)
{
	static const char* shakeTypeNames[MAX_SHAKE_TYPES] =
	{
		"Normal",
		"Vertical",
		"Directional",
		"Random",
	};
	
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	bool valueChanged = false;

	int shakeType = std::clamp(primTemplate->field_172 - 1, 0, MAX_SHAKE_TYPES - 1);

	if (ImGui::Combo("Shake Type", &shakeType, shakeTypeNames, MAX_SHAKE_TYPES))
	{
		primTemplate->field_172 = shakeType + 1;
		valueChanged = true;
	}

	valueChanged |= ImGui::DragFloat("Shake Scale", &primTemplate->field_168, 1.0F, 0.0F, 0.0F, "%g");
	valueChanged |= ImGui::DragFloat("Shake Speed", &primTemplate->field_164, 1.0F, 0.0F, 0.0F, "%g");
	return valueChanged;
}