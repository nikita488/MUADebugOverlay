#include "CTextureAssetCache.h"

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#include <Scene/ITextureCache.h>

void CTextureAssetCache::Initialize(EMemoryPoolID poolId)
{
	CAssetCache::Initialize(poolId);
	mDefaultTexture = TheTextureCache().CacheTexture("textures/default.png", MP_PERMANENT, true, true);
}

void CTextureAssetCache::Shutdown(EMemoryPoolID poolId)
{
	CAssetCache::Shutdown(poolId);
	mDefaultTexture.Nullify();
}

bool CTextureAssetCache::IsAssetTypeSupported(ICacheBase* cache) const
{
	return dynamic_cast<ITextureCache*>(cache);
}

std::string CTextureAssetCache::GetCacheKey(const char* assetPath) const
{
	ratl::string_vs<256> filename(assetPath);
	char* buf = filename.c_str();
	int len = filename.length();

	if (len >= 4 && buf[len - 4] == '.')
	{
		buf[len - 4] = '\0';
	}
	return filename.c_str();
}

bool CTextureAssetCache::CacheAsset(const char* cacheKey, EMemoryPoolID poolId) const
{
	ITexturePtr& texture = TheTextureCache().CacheTexture(cacheKey, poolId, true, true);
	return texture.Validate() && texture.Get() != mDefaultTexture.Get();
}

void CTextureAssetCache::FlushAsset(const char* cacheKey, EMemoryPoolID poolId) const
{
	TheTextureCache().FlushTexture(cacheKey, poolId);
}
