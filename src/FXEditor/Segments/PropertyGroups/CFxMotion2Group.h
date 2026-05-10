#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxMotion2Group : public IFxPropertyGroup
{
private:
	CSegmentVector mVelocity2;
	CSegmentVector mAcceleration2;
	CSegmentFloat mGravity2;
	CSegmentFloat mDrag2;

public:
	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
