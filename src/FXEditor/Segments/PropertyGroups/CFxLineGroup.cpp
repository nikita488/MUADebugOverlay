#include "CFxLineGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

CFxLineGroup::CFxLineGroup() :
	mNumSegments(0),
	mTaperEnabled(false),
	mTaperMode(0)
{
}

const char* CFxLineGroup::GetName() const
{
	return "Line";
}

void CFxLineGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	unsigned int flags = primTemplate->field_48;

	mNumSegments = primTemplate->field_218;
	mLength.Set(primTemplate->field_186);
	mChaos.Set(primTemplate->field_198);
	mStartVec.Set(primTemplate->field_222);
	mEndVec.Set(primTemplate->field_224);

	mTaperEnabled = (flags & (0x10 | 0x80000)) != 0;
	
	if ((flags & 0x10) != 0)
	{
		if ((flags & 0x80000) != 0)
		{
			mTaperMode = 1;
		}
		else
		{
			mTaperMode = 0;
		}
	}
	else if ((flags & 0x80000) != 0)
	{
		mTaperMode = 2;
	}
}

bool CFxLineGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& spawnFlags = primTemplate->field_56;
	unsigned int& flags = primTemplate->field_48;

	bool valueChanged = false;
	bool flagsChanged = false;

	valueChanged |= ImGui::CheckboxFlags("Repeat texture along line length", &flags, 0x40);

	{
		GroupSection section(IsChaosSupported(primType));
		valueChanged |= QuadraticGroup("Chaos", primTemplate->field_198, mChaos);
		valueChanged |= ImGui::CheckboxFlags("Set chaos per line segment", &flags, 0x20);
	}

	{
		GroupSection section(primType == FXPRIM_TYPE_LIGHTNING || primType == FXPRIM_TYPE_BEZIER);
		flagsChanged |= ImGui::Checkbox("Taper from", &mTaperEnabled); ImGui::SameLine();
		
		{
			GroupSection sectionB(mTaperEnabled);
			flagsChanged |= ImGui::RadioButton("Start", &mTaperMode, 0); ImGui::SameLine();
			flagsChanged |= ImGui::RadioButton("Center", &mTaperMode, 1); ImGui::SameLine();
			flagsChanged |= ImGui::RadioButton("End", &mTaperMode, 2);
		}
	}

	{
		GroupSection section(primType == FXPRIM_TYPE_BEZIER);
		
		ImGui::SeparatorText("Bezier Curve");
		
		int numSegments = primTemplate->field_218;
		
		if (ImGui::DragInt("Number of segments", &numSegments))
		{
			primTemplate->field_218 = numSegments;
			valueChanged = true;
		}

		valueChanged |= QuadraticGroup("Segment length", primTemplate->field_186, mLength, 1.0F, 0.0F, 0.0F, VALUE_FMT, SegmentQuadraticFlags_NoClamp);

		bool isOriginRelative = (spawnFlags & 0x1) == 0;
		bool isOrigin2Relative = (spawnFlags & 0x100) == 0;

		valueChanged |= VectorGroup("Start control point", primTemplate->field_222, mStartVec, 1.0F, 0.0F, 0.0f, VALUE_FMT, isOriginRelative ? SegmentVectorFlags_Relative : 0);
		valueChanged |= VectorGroup("End control point", primTemplate->field_224, mEndVec, 1.0F, 0.0F, 0.0f, VALUE_FMT, isOrigin2Relative ? SegmentVectorFlags_Relative : 0);
	}

	if (flagsChanged)
	{
		flags &= ~(0x10 | 0x80000);
		
		if (mTaperEnabled)
		{
			if (mTaperMode == 0)
			{
				flags |= 0x10;
			}
			else if (mTaperMode == 1)
			{
				flags |= 0x10;
				flags |= 0x80000;
			}
			else if (mTaperMode == 2)
			{
				flags |= 0x80000;
			}
		}
	}

	valueChanged |= flagsChanged;

	return valueChanged;
}