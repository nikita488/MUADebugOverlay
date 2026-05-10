#pragma once

#include "IFxPropertyGroup.h"
#include "../SegmentDataTypes.h"
#include "CBoltSettings.h"

enum EAxisType
{
	AXIS_ROTATION,
	AXIS_ORIENT,
};

class CSegmentAxis
{
private:
	CSegmentVector mAxis;
	EAxisType mAxisType;
	bool mRelative;
	int mRelativeAxis;
	bool mOverride;
	int mOverrideAxis;

public:
	CSegmentAxis(EAxisType type);

	void Set(CFxPrimitiveTemplate* primTemplate);
	bool Update(CFxPrimitiveTemplate* primTemplate);
};

class CFxOriginSizeGroup : public IFxPropertyGroup
{
private:
	CSegmentVector mOrigin;
	
	bool mOffsetEnabled;
	bool mAxisFromOffset;
	bool mLockHeightToRadius;
	int mOffsetType;
	int mOffsetOrient;
	CSegmentFloat mRadius;
	CSegmentFloat mRadius2;
	CSegmentFloat mHeight;

	CBoltSettings mBoltSettings;

	CSegmentQuadratic mSize;

	CSegmentQuadratic mRotation;
	CSegmentQuadratic mRotationRadius;

	CSegmentAxis mRotationAxis;
	CSegmentAxis mOrientAxis;

private:
	bool SpecialOffsetTypes(CFxPrimitiveTemplate* primTemplate);

public:
	CFxOriginSizeGroup();

	virtual const char* GetName() const;
	virtual void Initialize(IFxSegment* segment);
	virtual bool Draw(IFxSegment* segment);
};
