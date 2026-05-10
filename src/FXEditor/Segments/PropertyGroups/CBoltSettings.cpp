#include "CBoltSettings.h"

#include <imgui/imgui.h>
#include <FX/CFxPrimitiveTemplate.h>

#include "FxImGuiWidgets.h"

CBoltSettings::CBoltSettings(EBolt bolt) :
	mBolt(bolt),
	mEnabled(false),
	mBoltOntoEntFxOrigin(false),
	mUnboltImmediately(false),
	mUnboltUpAxis(false),
	mUnboltGradually(false)
{
}

void CBoltSettings::Set(CFxPrimitiveTemplate* primTemplate)
{
	unsigned int spawnFlags = primTemplate->field_56;
	unsigned int flags = primTemplate->field_48;
	unsigned int flags2 = primTemplate->field_52;

	if (mBolt == BOLT_1)
	{
		mEnabled = (spawnFlags & (0x1000000 | 0x8000000 | 0x20000000)) != 0;
		mBoltOntoEntFxOrigin = (flags & 0x80) != 0;
		mUnboltImmediately = (spawnFlags & 0x20000000) != 0;
		mUnboltUpAxis = (flags & 0x800) != 0;
		mUnboltGradually = (spawnFlags & 0x4000000) != 0;
	}
	else if (mBolt == BOLT_2)
	{
		mEnabled = (flags2 & (0x1 | 0x4 | 0x10)) != 0;
		mBoltOntoEntFxOrigin = (flags2 & 0x8) != 0;
		mUnboltImmediately = (flags2 & 0x4) != 0;
		mUnboltUpAxis = (flags2 & 0x20) != 0;
		mUnboltGradually = (flags2 & 0x2) != 0;
	}
}

bool CBoltSettings::Update(CFxPrimitiveTemplate* primTemplate)
{
	bool flagsChanged = false;

	unsigned int& spawnFlags = primTemplate->field_56;
	unsigned int& flags = primTemplate->field_48;
	unsigned int& flags2 = primTemplate->field_52;

	flagsChanged |= ImGui::Checkbox("Enable bolting", &mEnabled);

	{
		GroupSection sectionA(mEnabled);

		flagsChanged |= ImGui::Checkbox("Bolt onto entity effect origin", &mBoltOntoEntFxOrigin);
		//flagsChanged |= ImGui::Checkbox("Bolt onto camera focus point (broken)", &mBoltToCamFocus);
		flagsChanged |= ImGui::Checkbox("Unbolt immediately", &mUnboltImmediately);

		{
			GroupSection sectionB(!mUnboltImmediately);

			flagsChanged |= ImGui::Checkbox("Unbolt on Up axis", &mUnboltUpAxis);
			flagsChanged |= ImGui::Checkbox("Unbolt over lifetime", &mUnboltGradually);
		}
	}

	if (flagsChanged)
	{
		if (mBolt == BOLT_1)
		{
			spawnFlags &= ~(0x1000000 | 0x4000000 | 0x8000000 | 0x20000000);
			flags &= ~(0x80 | 0x800);

			if (mEnabled)
			{
				if (mBoltOntoEntFxOrigin)
				{
					flags |= 0x80;
				}

				if (mUnboltImmediately)
				{
					spawnFlags |= 0x20000000;
				}
				else
				{
					spawnFlags |= 0x1000000;

					if (mUnboltUpAxis)
					{
						flags |= 0x800;
					}

					if (mUnboltGradually)
					{
						spawnFlags |= 0x4000000;
					}
				}
			}
		}
		else if (mBolt == BOLT_2)
		{
			flags2 &= ~(0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20);

			if (mEnabled)
			{
				if (mBoltOntoEntFxOrigin)
				{
					flags2 |= 0x8;
				}

				if (mUnboltImmediately)
				{
					flags2 |= 0x4;
				}
				else
				{
					flags2 |= 0x1;

					if (mUnboltUpAxis)
					{
						flags2 |= 0x20;
					}

					if (mUnboltGradually)
					{
						flags2 |= 0x2;
					}
				}
			}
		}
	}

	return flagsChanged;
}
