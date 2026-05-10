#include "CAssetCache.h"

#include <Misc/ResourceSpecifier.h>
#include <Misc/CCacheHandlePool.h>

AssetRef::AssetRef() : 
	mRefCount(0)
{
}

void CAssetCache::Initialize(EMemoryPoolID poolId)
{
	auto& lookup = TheCacheHandlePool().mCacheHandleLookup;

	for (auto& it = lookup.begin(); it != lookup.end(); it++)
	{
		const SCacheHandle& cacheHandle = it.value();
		
		if (cacheHandle.mPoolId != poolId && IsAssetTypeSupported(cacheHandle.mConcrete))
		{
			ratl::string_vs<256> resSpec(cacheHandle.mName);
			const char* name = ResourceSpecifier_Name2(resSpec);

			mPermanentCache.emplace(name);
		}
	}
}

void CAssetCache::Shutdown(EMemoryPoolID poolId)
{	
	mCachedAssets.clear();
	mPermanentCache.clear();
}

bool CAssetCache::IsCachedPermanently(const char* assetPath) const
{
	return mPermanentCache.find(assetPath) != mPermanentCache.end();
}

bool CAssetCache::AddAsset(const char* assetPath, EMemoryPoolID poolId)
{
	const std::string& cacheKey = GetCacheKey(assetPath);
	
	if (IsCachedPermanently(cacheKey.c_str()))
	{
		return false;
	}

	if (CacheAsset(cacheKey.c_str(), poolId))
	{
		mCachedAssets[cacheKey].mRefCount++;
		return true;
	}

	return false;
}

bool CAssetCache::RemoveAsset(const char* assetPath, EMemoryPoolID poolId)
{
	const std::string& cacheKey = GetCacheKey(assetPath);
	
	if (IsCachedPermanently(cacheKey.c_str()))
	{
		return false;
	}

	auto& it = mCachedAssets.find(cacheKey.c_str());

	if (it != mCachedAssets.end())
	{
		AssetRef& ref = it->second;

		ref.mRefCount--;

		if (ref.mRefCount <= 0)
		{
			FlushAsset(cacheKey.c_str(), poolId);
			mCachedAssets.erase(it);
		}

		return true;
	}

	return false;
}
