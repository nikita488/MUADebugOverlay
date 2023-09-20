#pragma once

class CClient
{
public:
	void** field_0;//vftable
	float field_4;
	float field_8;
	float field_12;
	float field_16;
	unsigned char field_20;//flags
	float field_24;
	float field_28;
	float field_32;
	float field_36;
	unsigned char field_40;//flags
	float field_44;
	unsigned char field_48;//flags
};

//static_assert(sizeof(CClient) == 464, "Invalid structure size of CClient");

extern CClient &_TheClient;