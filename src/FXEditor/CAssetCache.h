#pragma once

#include <string>
#include <map>
#include <set>

#include "IAssetCache.h"

struct AssetRef
{
	int mRefCount;

	AssetRef();
};

class CAssetCache : public IAssetCache
{
public:
	std::map<std::string, AssetRef> mCachedAssets;
	std::set<std::string> mPermanentCache;

public:
	virtual void Initialize(EMemoryPoolID poolId);
	virtual void Shutdown(EMemoryPoolID poolId);

	virtual bool AddAsset(const char* assetPath, EMemoryPoolID poolId);
	virtual bool RemoveAsset(const char* assetPath, EMemoryPoolID poolId);

	virtual bool IsCachedPermanently(const char* assetPath) const;
	virtual bool IsAssetTypeSupported(ICacheBase* cache) const = 0;
	virtual std::string GetCacheKey(const char* assetPath) const = 0;
	virtual bool CacheAsset(const char* cacheKey, EMemoryPoolID poolId) const = 0;
	virtual void FlushAsset(const char* cacheKey, EMemoryPoolID poolId) const = 0;
};
