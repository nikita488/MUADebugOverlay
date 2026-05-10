#include "CFxAssetCache.h"

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#include <Misc/ResourceSpecifier.h>
#include <Misc/Common.h>

#include <FX/CFxTemplateManager.h>
#include <FX/CFxTemplate.h>

#include "IFxEditor.h"

void CFxAssetCache::Initialize(EMemoryPoolID poolId)
{
	auto& lookup = TheFxTemplateManager().field_0;

	for (auto& it = lookup.begin(); it != lookup.end(); it++)
	{
		const CFxTemplateManager::SFxFileRecord& record = it.key();
		CFxTemplate& fxTemplate = TheFxTemplateManager().field_19416[it.value()];

		if (fxTemplate.field_16 != poolId)
		{
			ratl::string_vs<256> resSpec(record.field_0.c_str());
			const char* name = ResourceSpecifier_Name2(resSpec);

			mPermanentCache.emplace(name);
		}
	}
}

void CFxAssetCache::Shutdown(EMemoryPoolID poolId)
{
	CAssetCache::Shutdown(poolId);
}

bool CFxAssetCache::IsCachedPermanently(const char* assetPath) const
{
	const std::string& fxCacheKey = TheFxEditor().GetFile().GetCacheKey();
	const char* name = ResourceSpecifier_Name2(fxCacheKey.c_str());

	return ratl::str::eql(assetPath, name) ||
		CAssetCache::IsCachedPermanently(assetPath);
}

bool CFxAssetCache::IsAssetTypeSupported(ICacheBase* cache) const
{
	return true;
}

std::string CFxAssetCache::GetCacheKey(const char* assetPath) const
{
	return Filename_FixFilename(assetPath, "effects/", ".xmlb");
}

bool CFxAssetCache::CacheAsset(const char* cacheKey, EMemoryPoolID poolId) const
{
	return TheFxTemplateManager().Load(cacheKey, poolId) != 0;
}

void CFxAssetCache::FlushAsset(const char* cacheKey, EMemoryPoolID poolId) const
{
	CFxTemplateManager& manager = TheFxTemplateManager();
	CFxTemplateManager::SFxFileRecord record(MakeResourceSpecifier(poolId, cacheKey));

	auto& it = manager.field_0.find(record);

	if (it != manager.field_0.end())
	{
		int handle = it.value();

		if (!handle)
		{
			return;
		}

		CFxTemplate& fxTemplate = manager.field_19416[handle];

		if (fxTemplate.field_16 == poolId)
		{
			fxTemplate.Free();
			manager.field_19416.free(handle);
			manager.field_0.erase(it);
		}
	}
}
