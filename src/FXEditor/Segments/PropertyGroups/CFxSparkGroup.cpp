#include "CFxSparkGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

const char* CFxSparkGroup::GetName() const
{
	return "Spark";
}

void CFxSparkGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	mLength.Set(primTemplate->field_186);
}

bool CFxSparkGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& flags = primTemplate->field_48;

	bool valueChanged = false;

	valueChanged |= QuadraticGroup("Length", primTemplate->field_186, mLength);
	valueChanged |= ImGui::CheckboxFlags("Scale length by velocity", &flags, 0x4);

	return valueChanged;
}