#include "CFxTransform2Group.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

#define MOVE_FLAGS SPRITE | ORIENTED_SPRITE | MODEL

bool IsMoveSupported(EFXPrimType primType)
{
	return IsPrimitiveSupported(MOVE_FLAGS, primType);
}

CFxTransform2Group::CFxTransform2Group() :
	mMoveFromStartToEnd(false),
	mBoltSettings(BOLT_2)
{
}

const char* CFxTransform2Group::GetName() const
{
	return "Transform2";
}

void CFxTransform2Group::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	unsigned int& flags = primTemplate->field_48;

	if (IsMoveSupported(primTemplate->field_4))
	{
		mMoveFromStartToEnd = (flags & 0x10) != 0;
	}

	mOrigin2.Set(primTemplate->field_208);
	mBoltSettings.Set(primTemplate);
	mSize2.Set(primTemplate->field_206);
}

bool CFxTransform2Group::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& spawnFlags = primTemplate->field_56;
	unsigned int& flags = primTemplate->field_48;
	unsigned int& flags2 = primTemplate->field_52;

	bool valueChanged = false;
	bool flagsChanged = false;

	{
		GroupSection sectionA(IsOrigin2Supported(primType));
		
		valueChanged |= ImGui::CheckboxFlags("Normalize effect axis", &spawnFlags, 0x400000);

		valueChanged |= RelativeVectorGroup("Origin2", primTemplate->field_208, mOrigin2, spawnFlags, 0x100);

		{
			GroupSection sectionB((spawnFlags & 0x400000) != 0);
			valueChanged |= CheckboxFlagsInverse("Use normalized effect axis", &spawnFlags, 0x800000);
		}

		valueChanged |= ImGui::CheckboxFlags("Use origin2 as offset from origin", &spawnFlags, 0x800);
		
		{
			GroupSection sectionA(IsMoveSupported(primType));
			flagsChanged |= ImGui::Checkbox("Move from start pt. to end pt.", &mMoveFromStartToEnd);
		}
	}

	{
		GroupSection section(IsLineBasedPrimitive(primType));

		ImGui::SeparatorText("Bolting2");
		valueChanged |= mBoltSettings.Update(primTemplate);
	}

	{
		GroupSection section(IsSize2Supported(primType));
		valueChanged |= QuadraticGroup("Size2", primTemplate->field_206, mSize2);
	}

	if (flagsChanged)
	{
		if (mMoveFromStartToEnd)
		{
			flags |= 0x10;
		}
		else
		{
			flags &= ~0x10;
		}
	}

	valueChanged |= flagsChanged;

	return valueChanged;
}