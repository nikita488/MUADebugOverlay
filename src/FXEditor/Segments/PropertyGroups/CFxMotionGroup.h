#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxMotionGroup : public IFxPropertyGroup
{
private:
	CSegmentVector mVelocity;
	CSegmentVector mAcceleration;
	CSegmentFloat mGravity;
	CSegmentFloat mDrag;

public:
	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
