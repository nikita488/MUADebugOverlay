#include "CFxLightGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"

const char* CFxLightGroup::GetName() const
{
	return "Light";
}

void CFxLightGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	
	mAttenuation.Set(primTemplate->field_200);
}

bool CFxLightGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	bool valueChanged = false;

	valueChanged |= QuadraticGroup("Attenuation", primTemplate->field_200, mAttenuation);
	return valueChanged;
}
