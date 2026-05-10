#include "CFxOriginSizeGroup.h"

#include <imgui/imgui.h>
#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

CSegmentAxis::CSegmentAxis(EAxisType type) :
	mAxisType(type),
	mRelative(true),
	mRelativeAxis(0),
	mOverride(false),
	mOverrideAxis(0)
{
}

void CSegmentAxis::Set(CFxPrimitiveTemplate* primTemplate)
{
	assert(primTemplate);

	unsigned int spawnFlags = primTemplate->field_56;
	unsigned int flags2 = primTemplate->field_52;
	
	if (mAxisType == AXIS_ROTATION)
	{
		mAxis.Set(primTemplate->field_194);

		mRelative = (spawnFlags & 0x80000000) == 0 || (flags2 & 0x100) != 0;
		mRelativeAxis = (flags2 & 0x100) != 0 ? 1 : 0;
		mOverride = (flags2 & (0x200 | 0x1000)) != 0;
		mOverrideAxis = (flags2 & 0x1000) != 0 ? 1 : 0;
	}
	else if (mAxisType == AXIS_ORIENT)
	{
		mAxis.Set(primTemplate->field_196);

		mRelative = (flags2 & 0x10000) == 0 || (flags2 & 0x2000) != 0;
		mRelativeAxis = (flags2 & 0x2000) != 0 ? 1 : 0;
		mOverride = (flags2 & (0x4000 | 0x8000)) != 0;
		mOverrideAxis = (flags2 & 0x8000) != 0 ? 1 : 0;
	}
}

bool CSegmentAxis::Update(CFxPrimitiveTemplate* primTemplate)
{
	static const char* relativeAxes[2] =
	{
		"Effect axis",
		"Camera axis",
	};
	
	unsigned int& spawnFlags = primTemplate->field_56;
	unsigned int& flags2 = primTemplate->field_52;

	int handle = 0;

	if (mAxisType == AXIS_ROTATION)
	{
		ImGui::SeparatorText("Rotation axis");
		handle = primTemplate->field_194;
	}
	else if(mAxisType == AXIS_ORIENT)
	{
		ImGui::SeparatorText("Orientation axis");
		handle = primTemplate->field_196;
	}

	ImGui::PushID(handle);

	bool valueChanged = false;

	{
		GroupSection section(!mOverride);
		valueChanged |= mAxis.Draw(handle);
	}

	//ImGui::Indent();

	bool flagsChanged = false;

	flagsChanged |= ImGui::Checkbox("Relative to", &mRelative); ImGui::SameLine();

	{
		GroupSection section(mRelative);
		//flagsChanged |= ImGui::Combo("##RelativeAxis", &mRelativeAxis, relativeAxes, 2);
		flagsChanged |= ImGui::RadioButton("Effect axis", &mRelativeAxis, 0); ImGui::SameLine();
		flagsChanged |= ImGui::RadioButton("Camera axis", &mRelativeAxis, 1);
	}

	{
		GroupSection section(mRelative && mRelativeAxis == 0);
		flagsChanged |= ImGui::Checkbox("Override axis to", &mOverride); ImGui::SameLine();
		
		{
			GroupSection sectionB(mOverride);

			//ImGui::Indent();
			flagsChanged |= ImGui::RadioButton("Velocity", &mOverrideAxis, 0); ImGui::SameLine();
			flagsChanged |= ImGui::RadioButton("Random", &mOverrideAxis, 1);
			//ImGui::Unindent();
		}
	}

	//ImGui::Unindent();
	ImGui::PopID();

	if (flagsChanged)
	{
		if (mAxisType == AXIS_ROTATION)
		{
			spawnFlags &= ~0x80000000;
			flags2 &= ~(0x100 | 0x200 | 0x1000);

			if (mRelative)
			{
				if (mRelativeAxis == 1)
				{
					flags2 |= 0x100;
				}
				else if (mOverride)
				{
					if (mOverrideAxis == 0)
					{
						flags2 |= 0x200;
					}
					else if (mOverrideAxis == 1)
					{
						flags2 |= 0x1000;
					}
				}
			}
			else
			{
				spawnFlags |= 0x80000000;
			}
		}
		else if (mAxisType == AXIS_ORIENT)
		{
			flags2 &= ~(0x2000 | 0x4000 | 0x8000 | 0x10000);

			if (mRelative)
			{
				if (mRelativeAxis == 1)
				{
					flags2 |= 0x2000;
				}
				else if (mOverride)
				{
					if (mOverrideAxis == 0)
					{
						flags2 |= 0x4000;
					}
					else if (mOverrideAxis == 1)
					{
						flags2 |= 0x8000;
					}
				}
			}
			else
			{
				flags2 |= 0x10000;
			}
		}
	}

	valueChanged |= flagsChanged;
	return valueChanged;
}

CFxOriginSizeGroup::CFxOriginSizeGroup() : 
	mOffsetEnabled(false),
	mAxisFromOffset(false),
	mLockHeightToRadius(false),
	mOffsetType(0),
	mOffsetOrient(0),
	mBoltSettings(BOLT_1),
	mRotationAxis(AXIS_ROTATION),
	mOrientAxis(AXIS_ORIENT)
{
}

const char* CFxOriginSizeGroup::GetName() const
{
	return "Transform";
}

void CFxOriginSizeGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	unsigned int spawnFlags = primTemplate->field_56;
	
	mOrigin.Set(primTemplate->field_176);

	mOffsetEnabled = (spawnFlags & (0x10 | 0x20)) != 0;
	mAxisFromOffset = (spawnFlags & 0x40) != 0;
	mLockHeightToRadius = (spawnFlags & 0x80) != 0;

	mOffsetType = (spawnFlags & 0x20) != 0 ? 1 : 0;
	
	if ((spawnFlags & 0x20000) != 0)
	{
		mOffsetOrient = 1;
	}
	else if ((spawnFlags & 0x8) != 0)
	{
		mOffsetOrient = 2;
	}

	//if ((spawnFlags & 0x10) != 0)
	//{
	//	mOffsetType = 0;
	//}
	//else if ((spawnFlags & 0x20) != 0)
	//{
	//	mOffsetType = 1;
	//}
	//
	//if ((spawnFlags & 0x20000) != 0)
	//{
	//	mOffsetOrient = 1;
	//}
	//else if ((spawnFlags & 0x8) != 0)
	//{
	//	mOffsetOrient = 2;
	//}

	mRadius.Set(primTemplate->field_76);
	mRadius2.Set(primTemplate->field_84);
	mHeight.Set(primTemplate->field_92);

	mBoltSettings.Set(primTemplate);

	mSize.Set(primTemplate->field_182);

	mRotation.Set(primTemplate->field_190);
	mRotationRadius.Set(primTemplate->field_192);

	mRotationAxis.Set(primTemplate);
	mOrientAxis.Set(primTemplate);
}

bool CFxOriginSizeGroup::SpecialOffsetTypes(CFxPrimitiveTemplate* primTemplate)
{
	assert(primTemplate);
	
	unsigned int& spawnFlags = primTemplate->field_56;

	ImGui::SeparatorText("Special Offset Types");

	bool valueChanged = false;
	bool flagsChanged = false;

	flagsChanged |= ImGui::Checkbox("Enable special offset types", &mOffsetEnabled);

	{
		GroupSection sectionA(mOffsetEnabled);

		flagsChanged |= ImGui::RadioButton("Spherical / Elliptical", &mOffsetType, 0);
		flagsChanged |= ImGui::RadioButton("Cylindrical", &mOffsetType, 1);
		valueChanged |= FloatGroup("Radius", primTemplate->field_76, mRadius);

		{
			GroupSection sectionB(mOffsetType == 1);
			valueChanged |= FloatGroup("Radius2", primTemplate->field_84, mRadius2);
		}

		{
			GroupSection sectionB(!mLockHeightToRadius);
			valueChanged |= FloatGroup("Height", primTemplate->field_92, mHeight);
		}

		flagsChanged |= ImGui::Checkbox("Set effect axis to offset direction", &mAxisFromOffset);

		{
			GroupSection sectionB(mOffsetType == 0);
			flagsChanged |= ImGui::Checkbox("Lock height to radius", &mLockHeightToRadius);
		}

		{
			bool relativeOrigin = (spawnFlags & 0x1) == 0;
			GroupSection sectionB(primTemplate->field_4 != FXPRIM_TYPE_PARTICLE_CLOUD && mOffsetType == 1 && relativeOrigin);

			static const char* orients[3] =
			{
				"Forward",
				"Right",
				"Up",
			};

			flagsChanged |= ImGui::Combo("Orient", &mOffsetOrient, orients, 3);
		}
	}

	if (flagsChanged)
	{
		spawnFlags &= ~(0x8 | 0x10 | 0x20 | 0x40 | 0x80 | 0x20000);
		
		if (mOffsetEnabled)
		{
			if (mOffsetType == 0)
			{
				spawnFlags |= 0x10;

				if (mLockHeightToRadius)
				{
					spawnFlags |= 0x80;
				}
			}
			else if (mOffsetType == 1)
			{
				spawnFlags |= 0x20;
			}

			if (mAxisFromOffset)
			{
				spawnFlags |= 0x40;
			}

			if (mOffsetOrient == 1)
			{
				spawnFlags |= 0x20000;
			}
			else if (mOffsetOrient == 2)
			{
				spawnFlags |= 0x8;
			}
		}
	}

	valueChanged |= flagsChanged;
	return valueChanged;
}

bool CFxOriginSizeGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& spawnFlags = primTemplate->field_56;

	bool valueChanged = false;
	
	{
		GroupSection section(primType != FXPRIM_TYPE_SCREENFLASH);
		valueChanged |= ImGui::CheckboxFlags("Normalize effect axis", &spawnFlags, 0x100000);
	}

	SegmentVectorFlags fieldFlags = SegmentVectorFlags_None;

	if (primType == FXPRIM_TYPE_SCREENFLASH)
	{
		fieldFlags |= SegmentVectorFlags_NoRelative;
	}

	valueChanged |= RelativeVectorGroup("Origin", primTemplate->field_176, mOrigin, spawnFlags, 0x1, 1.0F, 0.0F, 0.0F, VALUE_FMT, fieldFlags);
	
	{
		GroupSection sectionA(primType != FXPRIM_TYPE_SCREENFLASH);
		
		{
			GroupSection sectionB((spawnFlags & 0x100000) != 0);
			valueChanged |= CheckboxFlagsInverse("Use normalized effect axis", &spawnFlags, 0x200000);
		}

		{
			GroupSection sectionB(IsViewOffsetSupported(primType));
			valueChanged |= ImGui::DragFloat("View offset", &primTemplate->field_72, 1.0F, 0.0F, 0.0F, "%g");
		}

		valueChanged |= SpecialOffsetTypes(primTemplate);

		ImGui::SeparatorText("Bolting");
		valueChanged |= mBoltSettings.Update(primTemplate);
	}

	{
		GroupSection section(IsSizeSupported(primType));
		valueChanged |= QuadraticGroup("Size", primTemplate->field_182, mSize);
	}

	{
		GroupSection section(IsRotationSupported(primType));
		valueChanged |= QuadraticGroup("Rotation", primTemplate->field_190, mRotation, 1.0F, 0.0F, 0.0F, VALUE_FMT, SegmentQuadraticFlags_Angle);
	}

	{
		GroupSection section(IsRotationRadiusSupported(primType));
		valueChanged |= QuadraticGroup("Rotation radius", primTemplate->field_192, mRotationRadius);
	}

	{
		GroupSection section(IsRotationAxisSupported(primType));
		valueChanged |= mRotationAxis.Update(primTemplate);
	}

	{
		GroupSection section(IsOrientAxisSupported(primType));
		valueChanged |= mOrientAxis.Update(primTemplate);
	}

	return valueChanged;
}