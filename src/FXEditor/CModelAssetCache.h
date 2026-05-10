#pragma once

#include "CAssetCache.h"

class CModelAssetCache : public CAssetCache
{
public:
	virtual bool IsAssetTypeSupported(ICacheBase* cache) const;
	virtual std::string GetCacheKey(const char* assetPath) const;
	virtual bool CacheAsset(const char* cacheKey, EMemoryPoolID poolId) const;
	virtual void FlushAsset(const char* cacheKey, EMemoryPoolID poolId) const;
};
