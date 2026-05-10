#pragma once

#include "IFxPropertyGroup.h"

#include "../SegmentDataTypes.h"
#include "CBoltSettings.h"

class CFxTransform2Group : public IFxPropertyGroup
{
private:
	CSegmentVector mOrigin2;
	bool mMoveFromStartToEnd;
	CBoltSettings mBoltSettings;
	CSegmentQuadratic mSize2;

public:
	CFxTransform2Group();

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};

