#pragma once

#include "../rufl/hstring.h"
#include "CAttributes.h"
#include "igVec3f.h"
#include "igVec4f.h"

struct field_836_struct
{
	int field_0;
	void* field_4;//CIPoolInterface<class IPowerup, class CPowerup, 208, 256>
	float field_8;
	int field_12;
};

class CStats
{
public:
	CAttributes mAttributes;//CAttributes
	void* mStatData;//IStatsData pointer
	char field_292[20 + 1];//power1
	char field_313;
	char field_314[20 + 1];//power2
	char field_335;
	char field_336[20 + 1];//power3
	char field_357;
	char field_358[20 + 1];//power4
	char field_379;
	char field_380;//Current Skin index
	char field_381;//padding?
	char field_382;//padding?
	char field_383;//padding?
	int mBoltOns[7];//BoltOn handles from global BoltOn array
	int mStatEffects[7];//StatEffect handles from global StatEffect array
	int mStatEffectsSize;//StatEffect count
	char mName[32 + 1];//name
	char mCharacterName[32 + 1];//charactername/displayname
	char field_510[32 + 1];
	char mPowerstyle[32 + 1];//powerstyle
	char mMoveset[32 + 1];//moveset1
	char mCharacterAnims[32 + 1];//characteranims
	char mCharacterAnimClass[32 + 1];//characteranimsclass
	char mFootstepFX[32 + 1];//footstepfx
	char mSoundDir[32 + 1];//sounddir
	char field_742;//padding?
	char field_743;//padding?
	int field_744;
	char mSkinIndices[6];//skin indices
	char field_754;//padding?
	char field_755;//padding?
	hstring mSkinNames[6];//skin names
	char field_780;//character index
	char mHeaviness;//heaviness
	short field_782;//?index in SCharacterKey array
	char field_784;//?byte
	char mMenuLocation;//menulocation
	char field_786;//padding?
	char field_787;//padding?
	igVec3f mSize;//size
	int mTeam;//team index (28 - default, 29 - hero, 30 - enemy, 31 - altenemy)
	int mWeapon;//weapon index from global Weapon array
	int mComboLevel;//combolevel index (0 - basic, 1 - veryeasy, 2 - easy, 3 - medium, 4 - hard, 5 - popup, 6 - popsmash)
	char field_812;//?byte
	unsigned char field_813;//flags1 (1 - willflee, 2 - canFly, 4 - canSeeStealthed, 8 - large, 16 - teleportpathfail, 32 - xpexempt, 64 - ignoreboundsscaling, 128 - canbeallythrown)
	unsigned char field_814;//flags2 (1 - aiforceranged, 2 - ainomelee, 4 - ainostraffe, 8 - ainocover, 16 - , 32 - canthrowally, 64 - selectable, 128 - nonhumanoidskeleton)
	unsigned char field_815;//flags3 (1 - standard_pain_only, 2 - scaleattacks, 4 - targetheight, 8 - alwaysFlying, 16 - isteam, 32 - weapon_no_acc, 64 - , 128 - )
	float mScaleFactor;//scale_factor
	int field_820;
	int mMultipartManager;//?Multipart manager index
	int mCounter;//counter
	unsigned int field_832[(32 + 31) / 32];//?Talent heap mConstructed
	field_836_struct field_836[32];//?Talent heap mArray
	int field_1348;//?Talent heap mPush
	unsigned short mRace;//Race flags (2 - human, 4 - mutant, 8 - robot, 16 - sentinel, 32 - morlock, 64 - shadow, 128 - astral, 256 - xmen, 512 - brotherhood)
	short mAIClassType;//aiclasstype index (0 - brawler, 1 - affector , 2 - artillery , 3 - sentinel, 4 - support , 5 - flynonav, 6 - genericranged, 7 - genericrangednostraffe, 8 - rangedenemy, 9 - teleporter)
	short mAlertRadius;//alertradius
	short mAttackRange;//attackrange
	float mGrabChance;//grabchance
	float mPickupThrowChance;//pickupthrowchance
	int mScriptLevel;//scriptlevel
	float mFleeDistance;//fleeDistance
	float mMeleeTimerOffset;//meleetimeroffset
	float mMeleeTimerRandomAdd;//meleetimerrandomadd
	float mRangedTimerOffset;//rangedtimeroffset
	float mDangerRating;//dangerRating
	float mAIPickupWeaponChance;//aipickupweaponchance
};

static_assert(sizeof(CStats) == 1396, "Invalid structure size of CStats");