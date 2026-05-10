#pragma once

#include "IFxPropertyGroup.h"

class CFxOrientedSpriteGroup : public IFxPropertyGroup
{
public:
	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
