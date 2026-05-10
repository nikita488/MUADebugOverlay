#include "CFxMotion2Group.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"

const char* CFxMotion2Group::GetName() const
{
	return "Motion2";
}

void CFxMotion2Group::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	mVelocity2.Set(primTemplate->field_210);
	mAcceleration2.Set(primTemplate->field_212);
	mGravity2.Set(primTemplate->field_136);
	mDrag2.Set(primTemplate->field_128);
}

bool CFxMotion2Group::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	bool valueChanged = false;

	valueChanged |= RelativeVectorGroup("Velocity2", primTemplate->field_210, mVelocity2, primTemplate->field_56, 0x200);
	valueChanged |= RelativeVectorGroup("Acceleration2", primTemplate->field_212, mAcceleration2, primTemplate->field_56, 0x400);
	ImGui::Separator();
	valueChanged |= FloatGroup("Gravity2", primTemplate->field_136, mGravity2);
	valueChanged |= FloatGroup("Drag2", primTemplate->field_128, mDrag2);
	return valueChanged;
}