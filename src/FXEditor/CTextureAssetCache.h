#pragma once

#include "CAssetCache.h"

#include <Scene/ITexture.h>

class CTextureAssetCache : public CAssetCache
{
private:
	ITexturePtr mDefaultTexture;

public:
	virtual void Initialize(EMemoryPoolID poolId);
	virtual void Shutdown(EMemoryPoolID poolId);

	virtual bool IsAssetTypeSupported(ICacheBase* cache) const;
	virtual std::string GetCacheKey(const char* assetPath) const;
	virtual bool CacheAsset(const char* cacheKey, EMemoryPoolID poolId) const;
	virtual void FlushAsset(const char* cacheKey, EMemoryPoolID poolId) const;
};

