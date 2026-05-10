#pragma once

#include <vector>

#include "PropertyGroups/IFxPropertyGroup.h"

#include <FX/CFxPrimitiveTemplate.h>
#include <FX/FxDataTypes.h>
#include <tinyxml2/tinyxml2.h>

#include "IFxSegment.h"
#include "SegmentDataTypes.h"

class CFxSegment : public IFxSegment
{
protected:
	int mId;
	std::string mName;
	bool mUsePlatformOverrides;
	TPlatformFlags mPlatformFlags;
	bool mEnabled;
	std::string mDeathFxFile;
	std::string mIntervalFxFile;
	CFxPrimitiveTemplate* mTemplate;

	std::vector<IFxPropertyGroup*> mPropertyGroups;
	bool mResetSelectedTab;

protected:
	void UpdateAssetCaches(bool uncache = false);
	void SetAttribute(tinyxml2::XMLElement* element, const char* name, const char* value);
	void SetAttribute(tinyxml2::XMLElement* element, const char* name, int value);
	void SetAttribute(tinyxml2::XMLElement* element, const char* name, float value);
	void SetAttribute(tinyxml2::XMLElement* element, const char* name, unsigned int value);
	void SetAttribute(tinyxml2::XMLElement* element, const char* name, CRangedFloat& value);
	void SetAttribute(tinyxml2::XMLElement* element, const char* name, CRangedVector& value);
	void SetAttribute(tinyxml2::XMLElement* element, const char* name, CRangedQuadratic& value);

	void SetRangedVectorAttribute(tinyxml2::XMLElement* element, const char* name, int handle);
	void SetRangedQuadAttribute(tinyxml2::XMLElement* element, const char* name, int handle);
	void SetTextureAttribute(tinyxml2::XMLElement* element, const char* name, const ratl::string_vs<64>& texture);

public:
	CFxSegment();
	virtual ~CFxSegment();

	template<class T>
	T* AddPropertyGroup()
	{
		T* group = new T();
		mPropertyGroups.push_back(group);
		return group;
	}

	virtual void Initialize(CFxPrimitiveTemplate* primitive);
	
	virtual void InitializePropertyGroups();
	
	virtual void Cache();

	virtual void Uncache();

	virtual void Reset();

	virtual void Clone(IFxSegment* segment);

	virtual void Draw();

	virtual void SetAttributes(tinyxml2::XMLElement* element);

	inline virtual CFxPrimitiveTemplate* GetTemplate()
	{
		assert(mTemplate);
		return mTemplate;
	}

	virtual void SetId(int id)
	{
		mId = id;
	}

	virtual int GetId() const
	{
		return mId;
	}

	inline virtual void SetName(const char* name)
	{
		mName = name;
	}

	inline virtual const char* GetName() const
	{
		return mName.c_str();
	}

	inline virtual void SetUsePlatformOverrides(bool use)
	{
		mUsePlatformOverrides = use;
	}
	
	inline virtual bool GetUsePlatformOverrides() const
	{
		return mUsePlatformOverrides;
	}

	virtual void SetPlatformFlags(TPlatformFlags flags)
	{
		mPlatformFlags = flags;
	}

	virtual TPlatformFlags GetPlatformFlags() const
	{
		return mPlatformFlags;
	}

	virtual void SetPlatformSupported(EFxPlatform platform, bool supported);

	virtual bool IsPlatformSupported(EFxPlatform platform) const;

	virtual void SetFxLevelSupported(EFxLevel level, bool supported);

	virtual bool IsFxLevelSupported(EFxLevel level) const;

	virtual void SetEnabled(bool enabled);

	inline virtual bool GetEnabled() const
	{
		return mEnabled;
	}

	inline virtual void SetDeathFxFile(const char* filePath)
	{
		mDeathFxFile = filePath;
	}

	inline virtual const char* GetDeathFxFile() const
	{
		return mDeathFxFile.c_str();
	}

	virtual void SetIntervalFxFile(const char* filePath)
	{
		mIntervalFxFile = filePath;
	}

	virtual const char* GetIntervalFxFile() const
	{
		return mIntervalFxFile.c_str();
	}

	virtual void SetPrimitiveFlag(unsigned int flag, bool set);
};
