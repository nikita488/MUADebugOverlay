#pragma once

#include <string>
#include <vector>

#include "CFxFileInfo.h"
#include "Segments/IFxSegment.h"

class CFxEditor;
class CFxFileParser;
class CFxSegment;

typedef std::vector<IFxSegment*> TFxSegmentVector;

class CFxFile
{
	friend class CFxEditor;
	friend class SFxFileInfo;
	friend class CFxFileParser;
	friend class CFxSegment;

private:
	SFxFileInfo mFileInfo;
	std::string mName;
	bool mDirty;
	TFxSegmentVector mSegments;
	int mNextSegmentId;

private:
	void UpdateTemplateLinks(bool skipCheck = false);

	IFxSegment* AllocateFxSegment(EFXPrimType type);

	void LinkSegment(IFxSegment* segment, CFxPrimitiveTemplate* primitive, bool freeRangedComponents = true);

public:
	CFxFile();

	void Clear(bool free = true);

	bool Create();

	bool LoadFromFile(const char* fileName);

	bool SaveToFile(const char* fileName);

	bool CanAddSegment() const;

	inline bool HasSegments() const
	{
		return IsValid() && !mSegments.empty();
	}

	IFxSegment* NewSegment(EFXPrimType type);

	IFxSegment* CloneSegment(IFxSegment* segment);

	void DeleteSegment(IFxSegment* segment);

	inline bool IsValid() const
	{
		return mFileInfo.IsValid();
	}

	inline CFxTemplate* GetTemplate()
	{
		return mFileInfo.mTemplate;
	}

	inline int GetTemplateHandle() const
	{
		return mFileInfo.mTemplateHandle;
	}

	inline const std::string& GetCacheKey() const
	{
		return mFileInfo.mCacheKey;
	}

	inline void SetName(const char* name)
	{
		mName = name;
	}

	inline const char* GetName() const
	{
		return mName.c_str();
	}

	inline int GetSegmentCount() const
	{
		return int(mSegments.size());
	}

	inline IFxSegment* GetSegment(int index)
	{
		assert(index >= 0 && index < GetSegmentCount());
		return mSegments[index];
	}

	inline const IFxSegment* GetSegment(int index) const
	{
		assert(index >= 0 && index < GetSegmentCount());
		return mSegments[index];
	}

	inline void MarkDirty()
	{
		mDirty = true;
	}

	inline bool IsDirty() const
	{
		return mDirty;
	}
};
