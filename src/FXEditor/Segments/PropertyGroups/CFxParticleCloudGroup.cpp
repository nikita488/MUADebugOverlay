#include "CFxParticleCloudGroup.h"

#include <imgui/imgui.h>

#include "FxImGuiWidgets.h"

CFxParticleCloudGroup::CFxParticleCloudGroup() : 
	mNumParticles(0),
	mNoSync(false),
	mNewLifeMode(0)
{
}

const char* CFxParticleCloudGroup::GetName() const
{
	return "ParticleCloud";
}

void CFxParticleCloudGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	unsigned int spawnFlags = primTemplate->field_56;
	unsigned int flags2 = primTemplate->field_52;
	
	mNumParticles = primTemplate->field_218;

	mParticleSpawn.Set(primTemplate->field_156);
	mParticleTravel.Set(primTemplate->field_148);
	mTransformRotation.Set(primTemplate->field_220);

	if ((flags2 & 0x2000000) != 0)
	{
		if ((spawnFlags & 0x40000000) != 0)
		{
			mNewLifeMode = 1;
		}
		else
		{
			mNewLifeMode = 0;
		}
	}
	else
	{
		mNoSync = (spawnFlags & 0x40000000) != 0;
	}
}

bool CFxParticleCloudGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& spawnFlags = primTemplate->field_56;
	unsigned int& flags = primTemplate->field_48;
	unsigned int& flags2 = primTemplate->field_52;

	bool valueChanged = false;
	bool flagsChanged = false;

	if (ImGui::DragInt("Number of particles", &mNumParticles, 1.0F, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp))
	{
		primTemplate->field_218 = mNumParticles;
		valueChanged = true;
	}

	{
		GroupSection section(primType == FXPRIM_TYPE_PARTICLE_CLOUD);
		valueChanged |= ImGui::CheckboxFlags("Bolt emitter", &flags2, 0x8000000);
	}

	ImGui::SeparatorText("Life Options");

	bool setNewLife = (flags2 & 0x2000000) != 0;

	{
		GroupSection section(!setNewLife);
		valueChanged |= ImGui::DragFloat("Life scale", &primTemplate->field_144, 1.0F, 0.0F, 0.0F, "%g");
		flagsChanged |= ImGui::Checkbox("Randomized particle spawn##NoSync", &mNoSync);
	}

	flagsChanged |= ImGui::CheckboxFlags("Set new life", &flags2, 0x2000000);
	
	{
		GroupSection section(setNewLife);

		flagsChanged |= ImGui::RadioButton("Randomized particle spawn", &mNewLifeMode, 0);
		flagsChanged |= ImGui::RadioButton("Even particle distribution", &mNewLifeMode, 1);

		valueChanged |= FloatGroup("Spawn time", primTemplate->field_156, mParticleSpawn);
		valueChanged |= FloatGroup("Travel time", primTemplate->field_148, mParticleTravel);
	}

	ImGui::Separator();

	{
		GroupSection section(primType != FXPRIM_TYPE_WEATHERFX);
		
		valueChanged |= ImGui::CheckboxFlags("Transform rotation", &flags, 0x80000);

		{
			GroupSection sectionB((flags & 0x80000) != 0);
			SegmentVectorFlags rotFlags = SegmentVectorFlags_Angle;

			if ((spawnFlags & 0x1000) != 0)
			{
				rotFlags |= SegmentVectorFlags_NoY;
			}

			if ((spawnFlags & 0x2000) != 0)
			{
				rotFlags |= SegmentVectorFlags_NoZ;
			}

			valueChanged |= VectorGroup("Rotation", primTemplate->field_220, mTransformRotation, 1.0F, 0.0F, 0.0F, VALUE_FMT, rotFlags);
			valueChanged |= ImGui::CheckboxFlags("Lock Y to X", &spawnFlags, 0x1000);

			{
				GroupSection sectionB((spawnFlags & 0x1000) != 0);

				valueChanged |= ImGui::CheckboxFlags("Lock Z to X", &spawnFlags, 0x2000);
			}
		}
	}

	if (flagsChanged)
	{
		spawnFlags &= ~0x40000000;
		
		if ((flags2 & 0x2000000) != 0)
		{
			if (mNewLifeMode == 1)
			{
				spawnFlags |= 0x40000000;
			}
		}
		else
		{
			if (mNoSync)
			{
				spawnFlags |= 0x40000000;
			}
		}
	}

	valueChanged |= flagsChanged;

	return valueChanged;
}