#pragma once

#include "igVec3f.h"
#include "igVec4f.h"

struct field_28_struct
{
	unsigned short field_0;//?Talent index in the global Talents pool
	unsigned char field_2;//?start level
	unsigned char field_3;//?tier
	unsigned char field_4;
	unsigned char field_5;//start level
	char pad[2];
};

class CAttributes
{
public:
	void** field_0;//vtable
	short field_4;
	short field_6;
	unsigned char mStrength;//strength
	unsigned char mBody;//body
	unsigned char mMind;//mind
	char field_11;//padding?
	float field_12;
	float field_16;
	char mLevel;//level
	char mAILevel;//ailevel
	char mAIPower;//aipower
	char field_23;//padding?
	unsigned int field_24[(32 + 31) / 32];//Talent vector mConstructed
	field_28_struct field_28[32];//Talent vector mArray
	int field_284;//Talent vector mSize
};

static_assert(sizeof(CAttributes) == 288, "Invalid structure size of CAttributes");