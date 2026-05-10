#include "CFxOrientedSpriteGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

const char* CFxOrientedSpriteGroup::GetName() const
{
	return "OrientedSprite";
}

void CFxOrientedSpriteGroup::Initialize(IFxSegment* segment)
{
}

bool CFxOrientedSpriteGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	return ImGui::CheckboxFlags("Use as decal", &primTemplate->field_56, 0x8000);
}
