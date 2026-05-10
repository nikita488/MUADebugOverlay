#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxTrailGroup : public IFxPropertyGroup
{
public:
	CSegmentQuadratic mLength;
	CSegmentQuadratic mOffset;

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
