#pragma once

#include <FX/FxDataTypes.h>

#include <igGapMath.h>
#include <igMath/igVec4f.h>

#include "PropertyGroups/FxImGuiDefines.h"

using namespace Gap;

class CFxPrimitiveTemplate;

typedef int SegmentVectorFlags;
typedef int SegmentQuadraticFlags;

enum SegmentVectorFlags_
{
	SegmentVectorFlags_None = 0,
	SegmentVectorFlags_NoX = 1 << 0,
	SegmentVectorFlags_NoY = 1 << 1,
	SegmentVectorFlags_NoZ = 1 << 2,
	SegmentVectorFlags_NoRelative = 1 << 3,
	SegmentVectorFlags_Relative = 1 << 4,
	SegmentVectorFlags_Angle = 1 << 5,
};

enum SegmentQuadraticFlags_
{
	SegmentQuadraticFlags_None = 0,
	SegmentQuadraticFlags_NoStart = 1 << 0,
	SegmentQuadraticFlags_NoMid = 1 << 1,
	SegmentQuadraticFlags_NoEnd = 1 << 2,
	SegmentQuadraticFlags_NoClamp = 1 << 3,
	SegmentQuadraticFlags_Angle = 1 << 4,
};

enum ERangeType
{
	RANGE_MIN,
	RANGE_MAX,
};

enum EDragDataType
{
	DDT_FLOAT,
	DDT_ANGLE,
	DDT_MAX,
};

enum ETransitionType
{
	TRANSITION_CONSTANT,
	TRANSITION_LINEAR,
	TRANSITION_QUADRATIC,
	TRANSITION_MAX,
};

class CSegmentFloat
{
private:
	float mValue[2];

public:
	CSegmentFloat();

	void Set(const CRangedFloat& flt);

	void Update(CRangedFloat& flt);

	bool Draw(const char* label, CRangedFloat& flt, float speed = 1.0F, float min = 0.0F, float max = 0.0F, const char* format = VALUE_FMT);
};

class CSegmentVector
{
private:
	float mX[2];
	float mY[2];
	float mZ[2];

public:
	CSegmentVector();

	void Set(int handle);

	void Set(const CRangedVector& vec);

	void Update(CRangedVector& vec);

	bool Draw(int handle, float speed = 1.0F, float min = 0.0F, float max = 0.0F, const char* format = VALUE_FMT, SegmentVectorFlags flags = SegmentVectorFlags_None);
};

class CSegmentQuadratic
{
private:
	int mTransitionType;
	float mStart[2];
	float mEnd[2];
	float mMid[2];
	float mClamp[2];

private:
	void CalcQuad(ERangeType type, float& a, float& b, float& c);
	void CalcPoints(float a, float b, float c, float& start, float& mid, float& end);

public:
	CSegmentQuadratic();

	void Set(const CRangedQuadratic& quad);

	void Set(int handle);
	
	void Update(CRangedQuadratic& quad);

	bool Draw(int handle, float speed = 1.0F, float min = 0.0F, float max = 0.0F, const char* format = VALUE_FMT, SegmentQuadraticFlags flags = SegmentQuadraticFlags_None);
};

class CSegmentColor
{
private:
	int mTransitionType;
	igVec4f mStart[2];
	igVec4f mEnd[2];
	igVec4f mMid[2];

public:
	enum EColorType
	{
		COLOR_START,
		COLOR_MID,
		COLOR_END,
	};

	CSegmentColor();

	void Set(CFxPrimitiveTemplate* primitive);

	void Update(CFxPrimitiveTemplate* primitive, ERangeType range);

	bool Draw(CFxPrimitiveTemplate* primitive);
};
