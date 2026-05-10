#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxLineGroup : public IFxPropertyGroup
{
private:
	int mNumSegments;
	CSegmentQuadratic mLength;
	CSegmentQuadratic mChaos;
	CSegmentVector mStartVec;
	CSegmentVector mEndVec;

	bool mTaperEnabled;
	int mTaperMode;

public:
	CFxLineGroup();

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
