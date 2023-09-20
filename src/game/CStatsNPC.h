#pragma once

class CStatsNPC {
public:
	void** field_0;//vtable
	char field_4[32 + 1];//leadername (default - none)
	char field_37[32 + 1];//leaderpowerup
	char field_70[32 + 1];//deathnode
	short field_104;//leaderskin
	short field_106;//mutantskin
	char field_108;//mutatechance
	char field_109;//spawnorbs
	int field_112;//xpaward
	char field_116;//resurrect (1 - minion, 2 - master, 3 - supermaster)
	int field_120;//?Pattern Manager index
	int field_124;//specific_health
	float field_128;//npchealthscale
	float field_132;//npcdamagescale
	float field_136;//pain_pitch_range
	unsigned char field_140;//flags (1 bit - stats_from_level, 2 bit - vuln2_touch_of_death, 4 bit - no_ldr_scale, 8 bit - force_falling_push_response)
};

static_assert(sizeof(CStatsNPC) == 144, "Invalid structure size of CStatsNPC");