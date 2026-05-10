#include "CFxFileInfo.h"

#include <filesystem>

#include <Misc/Common.h>
#include <Misc/ResourceSpecifier.h>
#include <FX/CFxTemplateManager.h>

#include "IFxEditor.h"
#include "FxTemplateHelpers.h"

namespace fs = std::filesystem;

SFxFileInfo::SFxFileInfo()
{
	Clear();
}

void SFxFileInfo::SetFirstPrimitiveTemplate(CFxPrimitiveTemplate* primTemplate)
{
	assert(IsValid());
	mTemplate->field_0 = primTemplate;
}

void SFxFileInfo::SetMemoryPool(EMemoryPoolID poolId)
{
	assert(IsValid());
	mTemplate->field_16 = poolId;
}

bool SFxFileInfo::Allocate()
{
	assert(!IsValid());

	if (!CanAllocFxTemplate() || !mCacheKey.empty())
	{
		return false;
	}

	mTemplate = TheFxTemplateManager().AllocFxTemplate(&mTemplateHandle);

	if (!mTemplateHandle || !mTemplate)
	{
		return false;
	}

	SetMemoryPool(MP_MENU);
	return true;
}

void SFxFileInfo::Free()
{
	assert(IsValid());

	if (!mCacheKey.empty())
	{
		FreeFxTemplate(mCacheKey.c_str());
	}
	else
	{
		FreeFxTemplate(mTemplate, MP_MENU);
	}

	Clear();
}

//void SFxFileInfo::UpdateCacheKey(const char* fileName)
//{
//	//TODO: Set file extension to .xmlb
//	
//	if (!mCacheKey.empty())
//	{
//		return;
//	}
//
//	assert(IsValid());
//	
//	CFxTemplateManager& manager = TheFxTemplateManager();
//
//	//TODO: It will append effects/ if it's effects\\ bob
//	const char* name = Filename_FixFilename(fileName, "effects/", ".xmlb");
//	const char* resSpec = MakeResourceSpecifier(MP_MENU, name);
//	CFxTemplateManager::SFxFileRecord record(resSpec);
//
//	auto& it = manager.field_0.find(record);
//
//	if (it == manager.field_0.end())
//	{
//		manager.field_0.insert(record, mTemplateHandle);
//		mCacheKey = resSpec;
//	}
//}

void SFxFileInfo::UpdateCacheKey(const char* fileName)
{
	CFxTemplateManager& manager = TheFxTemplateManager();
	
	fs::path filePath = fileName;

	filePath.replace_extension();

	const char* fixedPath = Filename_FixFilename(filePath.generic_string().c_str(), "effects/", ".xmlb");
	const char* resSpec = MakeResourceSpecifier(MP_MENU, fixedPath);

	if (mCacheKey == resSpec)
	{
		return;
	}

	if (!mCacheKey.empty())
	{
		auto it = manager.field_0.find(mCacheKey.c_str());

		if (it != manager.field_0.end())
		{
			manager.field_0.erase(it);
		}
	}

	manager.field_0.insert(resSpec, mTemplateHandle);
	mCacheKey = resSpec;
}

CFxPrimitiveTemplate* SFxFileInfo::AllocatePrimitiveTemplate()
{
	assert(IsValid());
	return mTemplate->AllocFxPrimitiveTemplate();
}

void SFxFileInfo::FreePrimitiveTemplate(CFxPrimitiveTemplate* primTemplate)
{
	assert(IsValid());
	mTemplate->FreeFxPrimitiveTemplate(primTemplate);
}
