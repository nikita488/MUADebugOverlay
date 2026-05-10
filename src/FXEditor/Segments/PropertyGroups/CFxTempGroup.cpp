#include "CFxTempGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"

const char* CFxTempGroup::GetName() const
{
	return "Temp";
}

void CFxTempGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	
	mOrigin2.Set(primTemplate->field_208);
	mSize2.Set(primTemplate->field_206);

	mVelocity2.Set(primTemplate->field_210);
	mAcceleration2.Set(primTemplate->field_212);
	mDrag2.Set(primTemplate->field_128);
	mGravity2.Set(primTemplate->field_136);

	mScroll.Set(primTemplate->field_214);
	mScale.Set(primTemplate->field_216);
}

bool CFxTempGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	bool valueChanged = false;
	
	valueChanged |= RelativeVectorGroup("Endpoint", primTemplate->field_208, mOrigin2, primTemplate->field_56, 0x100);
	ImGui::Indent();
	valueChanged |= ImGui::CheckboxFlags("Use endpoint as offset from origin", &primTemplate->field_56, 0x800);
	ImGui::Unindent();

	valueChanged |= QuadraticGroup("Size2", primTemplate->field_206, mSize2);

	valueChanged |= RelativeVectorGroup("Velocity2", primTemplate->field_210, mVelocity2, primTemplate->field_56, 0x200);
	valueChanged |= RelativeVectorGroup("Acceleration2", primTemplate->field_212, mAcceleration2, primTemplate->field_56, 0x400);
	valueChanged |= FloatGroup("Drag2", primTemplate->field_128, mDrag2);
	valueChanged |= FloatGroup("Gravity2", primTemplate->field_136, mGravity2);

	valueChanged |= QuadraticGroup("Scroll", primTemplate->field_214, mScroll);
	valueChanged |= QuadraticGroup("Scale", primTemplate->field_216, mScale);
	return valueChanged;
}