#include "CCharStatsWindow.h"

#include <imgui/imgui.h>

#include <Game/CCharacters.h>
#include <Game/CEntity.h>

static bool AddCheckbox(const char* label, bool checked)
{
	return ImGui::Checkbox(label, &checked);
}

void CCharStatsWindow::Draw()
{
	if (ImGui::Begin("Character Stats"))
	{
		CCharacters& characters = (CCharacters&)TheCharacters();
		
		for (TStatPool::iterator it = characters.mStats.begin(); it != characters.mStats.end(); it++)
		{
			const CStats& stats = *it;
			int i = it.index();

			char label[64];
			snprintf(label, 64, "%02d: %s", i + 1, stats.mName.c_str());

			if (ImGui::TreeNode(label))
			{
				ImGui::Text("Strength: %d", stats.mAttributes.mData.mAttributeBaseLevels[0]);
				ImGui::Text("Body: %d", stats.mAttributes.mData.mAttributeBaseLevels[1]);
				ImGui::Text("Mind: %d", stats.mAttributes.mData.mAttributeBaseLevels[2]);
				ImGui::Text("Level: %d", stats.mAttributes.mData.mLevel);
				ImGui::Text("AI Level: %d", stats.mAttributes.mData.mAILevel);
				ImGui::Text("AI Power: %d", stats.mAttributes.mData.mAIPower);
				ImGui::Text("Talents: %d / %d", stats.mAttributes.mTalents.size(), stats.mAttributes.mTalents.capacity());
				ImGui::Text("Power 1: %s", stats.mPowerMap[0].Name());
				ImGui::Text("Power 2: %s", stats.mPowerMap[1].Name());
				ImGui::Text("Power 3: %s", stats.mPowerMap[2].Name());
				ImGui::Text("Power 4: %s", stats.mPowerMap[3].Name());
				ImGui::Text("Current Skin: %s", stats.mSkinNames[stats.mSkinSetIndex].c_str());

				unsigned int boltons = 0;

				for (unsigned int j = 0; j < 7; j++)
				{
					if (stats.mBoltOns[j] > 0)
						boltons++;
				}

				ImGui::Text("BoltOn's: %d / %d", boltons, 7);
				ImGui::Text("Stat Effect's: %d / %d", stats.mStatEffects.size(), stats.mStatEffects.capacity());
				ImGui::Text("Name: %s", stats.mName.c_str());
				ImGui::Text("Character Name: %s", stats.mDisplayName.c_str());
				ImGui::Text("Powerstyle: %s", stats.mPowerStyle.c_str());
				ImGui::Text("Moveset: %s", stats.mMoveSet.c_str());
				ImGui::Text("Character Anims: %s", stats.mCharacterAnimsName.c_str());
				ImGui::Text("Character Anims Class: %s", stats.mCharacterAnimsClassName.c_str());
				ImGui::Text("Footstep FX: %s", stats.mFootstepFX.c_str());
				ImGui::Text("Sound Dir: %s", stats.mSoundDir.c_str());

				ImGui::Text("Skin Class: %02d", stats.mSkinClass);

				if (ImGui::TreeNode("Skins"))
				{
					for (unsigned int j = 0; j < 6; j++)
					{
						if (!stats.mSkinNames[j].empty())
						{
							ImGui::Text("%02d: %s", j + 1, stats.mSkinNames[j].c_str());
						}
					}

					ImGui::TreePop();
				}

				ImGui::Text("Heaviness: %d", stats.mHeaviness);
				ImGui::Text("Menu Location: %d", stats.mMenuLocation);
				ImGui::Text("Size: %g %g %g", stats.mSize[0], stats.mSize[1], stats.mSize[2]);
				ImGui::Text("Team: %s", CEntity::GetStringFromTeam(EFlags(stats.mTeam)));
				ImGui::Text("Has Weapon: %s", stats.mWeapon > 0 ? "true" : "false");
				ImGui::Text("Combo Level: %d", stats.mComboLevel);

				if (ImGui::TreeNode("Flags"))
				{
					AddCheckbox("willflee", stats.mWillFlee);
					AddCheckbox("canFly", stats.mCanFly);
					AddCheckbox("canSeeStealthed", stats.mCanDetectStealthed);
					AddCheckbox("large", stats.mLarge);
					AddCheckbox("teleportpathfail", stats.mTeleportWhenPathFail);
					AddCheckbox("xpexempt", stats.mXPExempt);
					AddCheckbox("ignoreboundsscaling", stats.mIgnoreBoundsScaling);
					AddCheckbox("canbeallythrown", stats.mCanBeAllyThrown);

					AddCheckbox("aiforceranged", stats.mAIForceRanged);
					AddCheckbox("ainomelee", stats.mAINoMelee);
					AddCheckbox("ainostraffe", stats.mAINoStraffe);
					AddCheckbox("ainocover", stats.mAINoCover);
					AddCheckbox("Talent Values Dirty", stats.mTalentValuesDirty);
					AddCheckbox("canthrowally", stats.mCanThrowAlly);
					AddCheckbox("selectable", stats.mSelectable);
					AddCheckbox("nonhumanoidskeleton", stats.mNonHumanoid);

					AddCheckbox("standard_pain_only", stats.mStandardPainOnly);
					AddCheckbox("scaleattacks", stats.mScaleAttacks);
					AddCheckbox("targetheight", stats.mTargetHeight);
					AddCheckbox("alwaysFlying", stats.mAlwaysFlying);
					AddCheckbox("isteam", stats.mIsTeam);
					AddCheckbox("weapon_no_acc", stats.mWeaponNoAccessory);
					AddCheckbox("Powers Locked", stats.mPowersLocked);
					//AddCheckbox("reserved2", stats.padding);
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Race"))
				{
					unsigned int flags = unsigned int(stats.mRaces);

					ImGui::CheckboxFlags("Human", &flags, 2);
					ImGui::CheckboxFlags("Mutant", &flags, 4);
					ImGui::CheckboxFlags("Robot", &flags, 8);
					ImGui::CheckboxFlags("Sentinel", &flags, 16);
					ImGui::CheckboxFlags("Morlock", &flags, 32);
					ImGui::CheckboxFlags("Shadow", &flags, 64);
					ImGui::CheckboxFlags("Astral", &flags, 128);
					ImGui::CheckboxFlags("XMen", &flags, 256);
					ImGui::CheckboxFlags("Brotherhood", &flags, 512);
					ImGui::TreePop();
				}

				ImGui::Text("Scale Factor: %g", stats.mModelScaleFactor);
				ImGui::Text("Has Multipart's: %s", stats.mMultipartHandle > 0 ? "true" : "false");
				ImGui::Text("Counter: %d", stats.mCounter);
				ImGui::Text("AI Alert Radius: %d", stats.mAIAlertRadius);
				ImGui::Text("AI Attack Range: %d", stats.mAIAttackRange);
				ImGui::Text("AI Grab Chance: %g", stats.mAIGrabChance);
				ImGui::Text("AI Pickup Throw Chance: %g", stats.mAIPickUpThrowChance);
				ImGui::Text("AI Script Level: %d", stats.mAIScriptLevel);
				ImGui::Text("AI Flee Distance: %g", stats.mAIFleeDistance);
				ImGui::Text("AI Melee Timer Offset: %g", stats.mAIMeleeTimerOffset);
				ImGui::Text("AI Melee Timer Random Add: %g", stats.mAIMeleeTimerRandomAdd);
				ImGui::Text("AI Ranged Timer Offset: %g", stats.mAIRangedTimerOffset);
				ImGui::Text("AI Pickup Weapon Chance: %g", stats.mAIPickupWeaponChance);
				ImGui::Text("Danger Rating: %g", stats.mDangerRating);

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}