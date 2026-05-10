#pragma once

#include "../IFxSegment.h"

class IFxPropertyGroup
{
public:
	virtual const char* GetName() const = 0;
	virtual void Initialize(IFxSegment* segment) = 0;
	virtual bool Draw(IFxSegment* segment) = 0;
};
