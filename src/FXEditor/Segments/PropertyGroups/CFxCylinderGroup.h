#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxCylinderGroup : public IFxPropertyGroup
{
private:
	int mNumSegments;
	CSegmentQuadratic mStartArc;
	CSegmentQuadratic mEndArc;

public:
	CFxCylinderGroup();

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
