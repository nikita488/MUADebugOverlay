#pragma once

struct field_16_struct
{
	int field_0;
	int field_4;
	unsigned int field_8;
};

class CCheats
{
public:
	void** field_0;//vtable
	int field_4;
	unsigned int field_8;//bitfield
	unsigned int field_12;//bitfield
	field_16_struct field_16[8];//mData
	int field_112[8];//mFree
	int field_144;//mPush
	int field_148;//mPop
	int field_152;//mSize
	int field_156;
	int field_160;
	int field_164;
	int field_168;
	float field_172;
	unsigned int field_176;//bitfield or count
	char field_180;//byte OnlineCheats
	char field_181;//byte flags?
	char field_182;//padding?
	char field_183;//padding?
};

static_assert(sizeof(CCheats) == 184, "Invalid structure size of CCheats");

CCheats* TheCheats();