#pragma once

#include "../rufl/hstring.h"
#include "igVec4f.h"

class CStatsHero {
public:
	void** field_0;//vtable
	int field_4;//last ItemInstance index
	void* field_8;//CPoolInterface<class IItemInstance, class CItemInstance, 366>*
	int field_12;//is any item equipped 0 - no, 1 - yes
	igVec4f field_16;//gen_charge_color
	hstring field_32;//autospend
	int field_36;//level_every_skill_reward
	float field_40;//?health
	float field_44;//?energy
	float field_48;//?xtreme progress 0..1
	int field_52;
	char field_56;//flags?
	unsigned int field_60;//xp
	int field_64;
	unsigned char field_68;//wolverine charge progress, 0..255
	float field_72;//remaining revive time (in seconds?)
	char field_76;//?level
	char field_77;
	char field_78;
	char field_79;
	char field_80;
	char field_81;
	char field_82;
	char field_83;
	char field_84;
	char field_85;//?flags (16 - platform_hidden)
};

static_assert(sizeof(CStatsHero) == 88, "Invalid structure size of CStatsHero");