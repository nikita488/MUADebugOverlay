#pragma once

#include "CStats.h"
#include "CStatsHero.h"
#include "CStatsNPC.h"

#define MAX_STATS MAX_HEROS + 11

struct field_54800_struct
{
	hstring field_0;//name
	hstring field_4;//charactername
	hstring field_8;//skin
	hstring field_12;//characteranims
	int field_16;//?CCharacters CStats mHandles index
	unsigned char field_20;//level
	unsigned char field_21;//team index
	unsigned short field_22;//textureicon
	char field_24;//-1 always?
	unsigned char field_25;//?flags (1 - , 2 - playable (116 or 84 ), 4 - , 32 - , 64 - )
	char field_26;//padding?
	char field_27;//padding?
};

class tree_node
{
	int mParent;
	int mLeft;
	int mRight;
};

struct SCharacterKey
{
	hstring field_12;//name (lower cased)
	unsigned char field_16;
	unsigned char field_17;
	unsigned char field_18;
	unsigned char field_19;
};

struct SNode
{
	tree_node nodeData;
	SCharacterKey value;
};

struct CHero
{
	CAttributes field_0;
	CStatsHero field_288;
	char field_376[20 + 1];
	char field_397;
	char field_398[20 + 1];
	char field_418;
	char field_419[20 + 1];
	char field_439;
	char field_440[20 + 1];
	char field_463;
	char field_464;//byte
	char field_465;//padding?
	char field_466;//padding?
	char field_467;//padding?

	int field_468;
	char field_472;//byte
	char field_473;//byte
	char field_474;//byte
	char field_475;//byte
	char field_476;//byte
	char field_477;//padding?
	char field_478;//padding?
	char field_479;//padding?
	int field_480;
	int field_484;
	int field_488;
	int field_492;
	int field_496;
	int field_500;
	int field_504;
	int field_508;
	int field_512;
	int field_516;
	int field_520;
	int field_524;
	int field_528;
	int field_532;
	int field_536;
	int field_540;
};

static_assert(sizeof(CHero) == 544, "Invalid structure size of Hero");

class CCharacters
{
public:
	void** field_0;//vftable
	int field_4[(MAX_STATS + 31) / 32];
	CStats field_12[MAX_STATS];//mData
	int field_54456[MAX_STATS];//mFree
	int field_54612;//mPush
	int field_54616;//mPop
	int field_54620;//mSize
	unsigned int field_54624[(MAX_STATS + 31) / 32];//mUsed
	int field_54632;//mSize
	int field_54636[MAX_STATS];//mHandles
	int field_54792;//mMASK_HANDLE_TO_INDEX
	int field_54796;//mMASK_NUM_BITS

	field_54800_struct field_54800[350];
	int field_64600;//mSize

	void** field_64604;//vftable
	int field_64608;//mRoot
	int field_64612;//mLastAdd
	SNode field_64616[350];//mData
	int field_71616[350];//mFree
	int field_73016;//mPush
	int field_73020;//mPop
	int field_73024;//mSize
	unsigned int field_73028[(350 + 31) / 32];//mUsed
	int field_73072;//mSize
	short field_73076[350];//mValues

	//map_base iterator?
	int field_73776;//mLoc
	void* field_73780;//mOwner

	CHero field_73784[MAX_HEROS];
	CHero field_89016[MAX_HEROS];

	bool field_104248;
	bool field_104249;

	unsigned int field_104252[(MAX_HEROS + 31) / 32];//mConstructed
	CStatsHero field_104256[MAX_HEROS];//mData
	int field_106720[MAX_HEROS];//mFree
	int field_106832;//mPush
	int field_106836;//mPop
	int field_106840;//mSize
	unsigned int field_106844[(MAX_HEROS + 31) / 32];//mUsed
	int field_106848;//mSize

	unsigned int field_106852[(13 + 31) / 32];//mConstructed
	CStatsNPC field_106856[13];//mData
	int field_108728[13];//mFree
	int field_108780;//mPush
	int field_108784;//mPop
	int field_108788;//mSize
	unsigned int field_108792[(13 + 31) / 32];//mUsed
	int field_108796;//mSize

	short field_108800[350];
	int field_109500;//mSize

	short field_109504[350];
	int field_110204;//mSize
};

#if MAX_HEROS == 28
	static_assert(sizeof(CCharacters) == 110208, "Invalid structure size of CCharacters");
#elif MAX_HEROS == 52
	static_assert(sizeof(CCharacters) == 172232, "Invalid structure size of CCharacters");
#endif

CCharacters* TheCharacters();