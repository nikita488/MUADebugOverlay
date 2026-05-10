#pragma once

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxGenerationGroup : public IFxPropertyGroup
{
private:
	ratl::string_vs<64> mName;
	CSegmentFloat mDelay;
	CSegmentFloat mCount;
	CSegmentFloat mLife;
	ratl::string_vs<64> mDeathFxFile;
	ratl::string_vs<64> mIntervalFxFile;
	int mIntervalMode;

private:
	bool PlayOptions(IFxSegment* segment);
	bool DeathEffect(IFxSegment* segment);
	bool IntervalEffect(IFxSegment* segment, bool& flagsChanged);
	bool PlatformOverrides(IFxSegment* segment);
	bool FxLevels(IFxSegment* segment);

public:
	CFxGenerationGroup();

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
