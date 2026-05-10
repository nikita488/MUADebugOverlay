#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"

class CFxFlagsGroup : public IFxPropertyGroup
{
private:

public:
	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
