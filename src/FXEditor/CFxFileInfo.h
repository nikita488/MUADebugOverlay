#pragma once

#include <string>

#include <Misc/IMemory.h>
#include <FX/CFxTemplate.h>

class SFxFileInfo
{
public:
	CFxTemplate* mTemplate;
	int mTemplateHandle;
	std::string mCacheKey;

public:
	void SetFirstPrimitiveTemplate(CFxPrimitiveTemplate* primTemplate);
	void SetMemoryPool(EMemoryPoolID poolId);

public:
	SFxFileInfo();

	inline void Clear()
	{
		mTemplate = NULL;
		mTemplateHandle = 0;
		mCacheKey.clear();
	}

	inline bool IsValid() const
	{
		return mTemplateHandle && mTemplate;
	}

	bool Allocate();
	void Free();
	void UpdateCacheKey(const char* fileName);

	CFxPrimitiveTemplate* AllocatePrimitiveTemplate();
	void FreePrimitiveTemplate(CFxPrimitiveTemplate* primTemplate);

	inline bool IsPersistLoop() const
	{
		return mTemplate && mTemplate->field_12;
	}
};
