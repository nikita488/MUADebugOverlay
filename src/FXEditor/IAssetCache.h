#pragma once

#include <Misc/ICacheBase.h>

class IAssetCache
{
public:
	virtual void Initialize(EMemoryPoolID poolId) = 0;
	virtual void Shutdown(EMemoryPoolID poolId) = 0;

	virtual bool AddAsset(const char* assetPath, EMemoryPoolID poolId) = 0;
	virtual bool RemoveAsset(const char* assetPath, EMemoryPoolID poolId) = 0;
};

