#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxTempGroup : public IFxPropertyGroup
{
private:
	CSegmentVector mOrigin2;
	CSegmentQuadratic mSize2;

	CSegmentVector mVelocity2;
	CSegmentVector mAcceleration2;
	CSegmentFloat mDrag2;
	CSegmentFloat mGravity2;

	CSegmentQuadratic mScroll;
	CSegmentQuadratic mScale;

public:
	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
