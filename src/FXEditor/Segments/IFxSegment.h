#pragma once

#include <string>

#include <FX/CFxPrimitiveTemplate.h>

namespace tinyxml2
{
	class XMLElement;
};

enum EFxPlatform
{
	PLATFORM_WII,
	PLATFORM_PS2,
	PLATFORM_XBOX,
	PLATFORM_XENON,
	PLATFORM_PSP,
	PLATFORM_PS3,
	PLATFORM_ALL,
	PLATFORM_MAX,
	PLATFORM_FIRST = PLATFORM_WII,
	PLATFORM_LAST = PLATFORM_ALL,
	PLATFORM_COUNT = PLATFORM_LAST - PLATFORM_FIRST,
};

enum EFxLevel
{
	LEVEL_ALL,
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	LEVEL_6,
	LEVEL_7,
	LEVEL_8,
	LEVEL_MAX,
	LEVEL_FIRST = LEVEL_1,
	LEVEL_LAST = LEVEL_MAX,
	LEVEL_COUNT = LEVEL_LAST - LEVEL_FIRST,
};

typedef unsigned int TPlatformFlags;

class IFxSegment
{
public:
	virtual ~IFxSegment() {};

	virtual void Initialize(CFxPrimitiveTemplate* primitive) = 0;

	virtual void Reset() = 0;

	virtual void Clone(IFxSegment* segment) = 0;

	virtual void Draw() = 0;
	
	virtual void SetAttributes(tinyxml2::XMLElement* element) = 0;
	
	virtual CFxPrimitiveTemplate* GetTemplate() = 0;
	
	virtual void SetId(int id) = 0;
	virtual int GetId() const = 0;

	virtual void SetName(const char* name) = 0;
	virtual const char* GetName() const = 0;
	
	virtual void SetUsePlatformOverrides(bool use) = 0;
	virtual bool GetUsePlatformOverrides() const = 0;

	virtual void SetPlatformFlags(TPlatformFlags flags) = 0;
	virtual TPlatformFlags GetPlatformFlags() const = 0;

	virtual void SetPlatformSupported(EFxPlatform platform, bool supported) = 0;
	virtual bool IsPlatformSupported(EFxPlatform platform) const = 0;

	virtual void SetFxLevelSupported(EFxLevel level, bool supported) = 0;
	virtual bool IsFxLevelSupported(EFxLevel level) const = 0;

	virtual void SetEnabled(bool enabled) = 0;
	virtual bool GetEnabled() const = 0;

	virtual void SetDeathFxFile(const char* filePath) = 0;
	virtual const char* GetDeathFxFile() const = 0;

	virtual void SetIntervalFxFile(const char* filePath) = 0;
	virtual const char* GetIntervalFxFile() const = 0;

	virtual void SetPrimitiveFlag(unsigned int flag, bool set) = 0;
};
