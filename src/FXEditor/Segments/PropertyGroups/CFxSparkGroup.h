#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxSparkGroup : public IFxPropertyGroup
{
private:
	CSegmentQuadratic mLength;

public:
	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
