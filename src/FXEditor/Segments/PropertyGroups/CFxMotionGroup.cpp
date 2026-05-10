#include "CFxMotionGroup.h"

#include <imgui/imgui.h>

#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

const char* CFxMotionGroup::GetName() const
{
	return "Motion";
}

void CFxMotionGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	
	mVelocity.Set(primTemplate->field_178);
	mAcceleration.Set(primTemplate->field_180);
	mGravity.Set(primTemplate->field_40);
	mDrag.Set(primTemplate->field_32);
}

bool CFxMotionGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	bool valueChanged = false;

	SegmentVectorFlags fieldFlags = SegmentVectorFlags_None;

	if (primType == FXPRIM_TYPE_SCREENFLASH)
	{
		fieldFlags |= SegmentVectorFlags_NoRelative;
	}

	if (IsParticleBasedPrimitive(primType))
	{
		bool isSpecialOffsetEnabled = (primTemplate->field_56 & (0x10 | 0x20)) != 0;
		bool isAxisFromOffset = (primTemplate->field_56 & 0x40) != 0;
		
		if (isSpecialOffsetEnabled && isAxisFromOffset)
		{
			fieldFlags |= SegmentVectorFlags_NoY;
			fieldFlags |= SegmentVectorFlags_NoZ;
		}
	}

	valueChanged |= RelativeVectorGroup("Velocity", primTemplate->field_178, mVelocity, primTemplate->field_56, 0x2, 1.0F, 0.0F, 0.0F, VALUE_FMT, fieldFlags);
	valueChanged |= RelativeVectorGroup("Acceleration", primTemplate->field_180, mAcceleration, primTemplate->field_56, 0x4, 1.0F, 0.0F, 0.0F, VALUE_FMT, fieldFlags);
	
	ImGui::Separator();
	
	{
		GroupSection section(IsGravitySupported(primType));
		valueChanged |= FloatGroup("Gravity", primTemplate->field_40, mGravity);
	}

	{
		GroupSection section(IsDragSupported(primType));
		valueChanged |= FloatGroup("Drag", primTemplate->field_32, mDrag);
	}

	return valueChanged;
}