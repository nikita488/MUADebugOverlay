#include "CFxCylinderGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"

CFxCylinderGroup::CFxCylinderGroup() :
	mNumSegments(0)
{
}

const char* CFxCylinderGroup::GetName() const
{
	return "Cylinder";
}

void CFxCylinderGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	
	mNumSegments = primTemplate->field_218;
	mStartArc.Set(primTemplate->field_202);
	mEndArc.Set(primTemplate->field_204);
}

bool CFxCylinderGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	unsigned int& flags = primTemplate->field_48;

	bool valueChanged = false;

	int numSegments = primTemplate->field_218;

	if (ImGui::DragInt("Number of sides", &numSegments, 1.0F, 0, IG_INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
	{
		primTemplate->field_218 = numSegments;
		valueChanged = true;
	}

	valueChanged |= QuadraticGroup("Start arc", primTemplate->field_202, mStartArc, 1.0F, 0.0F, 0.0F, VALUE_FMT, SegmentQuadraticFlags_Angle);
	valueChanged |= QuadraticGroup("End arc", primTemplate->field_204, mEndArc, 1.0F, 0.0F, 0.0F, VALUE_FMT, SegmentQuadraticFlags_Angle);

	valueChanged |= ImGui::CheckboxFlags("Use size2 as taper angle", &flags, 0x8);
	return valueChanged;
}