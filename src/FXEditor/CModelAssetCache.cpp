#include "CModelAssetCache.h"

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#include <Misc/CCacheHandlePool.h>
#include <Scene/IIGBInfoCache2.h>
#include <injector.h>

void MakeIGBFilepath(const char* filePath, ratl::string_vs<256>& igbFilePath)
{
	Call<0x67CA90, const char*, ratl::string_vs<256>&>(filePath, igbFilePath);
}

bool CModelAssetCache::IsAssetTypeSupported(ICacheBase* cache) const
{
	return dynamic_cast<IIGBInfoCache2*>(cache);
}

std::string CModelAssetCache::GetCacheKey(const char* assetPath) const
{
	ratl::string_vs<256> igbPath;
	MakeIGBFilepath(assetPath, igbPath);
	return igbPath.c_str();
}

bool CModelAssetCache::CacheAsset(const char* assetPath, EMemoryPoolID poolId) const
{
	return TheIGBInfoCache2().CacheIGBInfo(assetPath, poolId) != NULL;
}

void CModelAssetCache::FlushAsset(const char* assetPath, EMemoryPoolID poolId) const
{
	TheCacheHandlePool().Uncache(assetPath, poolId, &TheIGBInfoCache2());
}
