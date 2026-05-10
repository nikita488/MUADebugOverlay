#pragma once

#include "Segments/IFxSegment.h"

class CFxSegmentFilter
{
private:
	EFxPlatform mPlatform;
	EFxLevel mFxLevel;

public:
	CFxSegmentFilter();

	void Reset();

	bool Matches(IFxSegment* segment);

	bool Update();
};
