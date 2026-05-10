#include "CFxTrailGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

const char* CFxTrailGroup::GetName() const
{
	return "Trail";
}

void CFxTrailGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	mLength.Set(primTemplate->field_186);
	mOffset.Set(primTemplate->field_188);
}

bool CFxTrailGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	unsigned int& spawnFlags = primTemplate->field_56;
	bool isBolted = (spawnFlags & (0x1000000 | 0x8000000 | 0x20000000)) != 0;

	bool valueChanged = false;

	valueChanged |= CheckboxFlagsInverse("Enable taper", &primTemplate->field_48, 0x10);

	valueChanged |= QuadraticGroup("Segment max length", primTemplate->field_186, mLength);
	
	{
		GroupSection section(isBolted);
		valueChanged |= QuadraticGroup("Bolt origin offset", primTemplate->field_188, mOffset);
	}

	return valueChanged;
}
