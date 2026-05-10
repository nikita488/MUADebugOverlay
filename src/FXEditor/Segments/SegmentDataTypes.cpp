#include "SegmentDataTypes.h"

#include <algorithm>
#include <cmath>
#include <imgui/imgui.h>

#include <FX/CFxPrimitiveTemplate.h>
#include <FX/CRangedComponentPool.h>

#include <igMath/igMath.h>

#include "PropertyGroups/FxImGuiWidgets.h"

CSegmentFloat::CSegmentFloat()
{
	mValue[RANGE_MIN] = 0.0F;
	mValue[RANGE_MAX] = 0.0F;
}

void CSegmentFloat::Set(const CRangedFloat& flt)
{
	mValue[RANGE_MIN] = flt._base;
	mValue[RANGE_MAX] = flt._base + flt._range;
}

void CSegmentFloat::Update(CRangedFloat& flt)
{
	flt._base = mValue[RANGE_MIN];
	flt._range = mValue[RANGE_MAX] - mValue[RANGE_MIN];
}

bool CSegmentFloat::Draw(const char* label, CRangedFloat& flt, float speed, float min, float max, const char* format)
{
	bool valueChanged = false;
	
	if (ImGui::DragFloat2(label, mValue, speed, min, max, format))
	{
		Update(flt);
		valueChanged = true;
	}

	return valueChanged;
}

CSegmentVector::CSegmentVector()
{
	mX[RANGE_MIN] = 0.0F;
	mY[RANGE_MIN] = 0.0F;
	mZ[RANGE_MIN] = 0.0F;

	mX[RANGE_MAX] = 0.0F;
	mY[RANGE_MAX] = 0.0F;
	mZ[RANGE_MAX] = 0.0F;
}

void CSegmentVector::Set(int handle)
{
	Set(TheVectorPool().Get(handle));
}

void CSegmentVector::Set(const CRangedVector& vec)
{
	mX[RANGE_MIN] = vec._base[0];
	mY[RANGE_MIN] = vec._base[1];
	mZ[RANGE_MIN] = vec._base[2];

	mX[RANGE_MAX] = vec._base[0] + vec._range[0];
	mY[RANGE_MAX] = vec._base[1] + vec._range[1];
	mZ[RANGE_MAX] = vec._base[2] + vec._range[2];
}

void CSegmentVector::Update(CRangedVector& vec)
{
	vec._base.set(mX[RANGE_MIN], mY[RANGE_MIN], mZ[RANGE_MIN]);
	vec._range[0] = mX[RANGE_MAX] - mX[RANGE_MIN];
	vec._range[1] = mY[RANGE_MAX] - mY[RANGE_MIN];
	vec._range[2] = mZ[RANGE_MAX] - mZ[RANGE_MIN];
}

bool CSegmentVector::Draw(int handle, float speed, float min, float max, const char* format, SegmentVectorFlags flags)
{
	EDragDataType dataType = (flags & SegmentVectorFlags_Angle) != 0 ? DDT_ANGLE : DDT_FLOAT;
	bool relative = (flags & SegmentVectorFlags_NoRelative) == 0 && (flags & SegmentVectorFlags_Relative) != 0;
	
	const char* labelX = relative ? "Forward" : "X";
	const char* labelY = relative ? "Right"   : "Y";
	const char* labelZ = relative ? "Up"      : "Z";
	
	bool valueChanged = false;

	{
		GroupSection section((flags & SegmentVectorFlags_NoX) == 0);
		
		if (DragRange(labelX, dataType, mX, speed, min, max, format))
		{
			Update(TheVectorPool().Get(handle));
			valueChanged = true;
		}
	}

	{
		GroupSection section((flags & SegmentVectorFlags_NoY) == 0);
		
		if (DragRange(labelY, dataType, mY, speed, min, max, format))
		{
			Update(TheVectorPool().Get(handle));
			valueChanged = true;
		}
	}

	{
		GroupSection section((flags & SegmentVectorFlags_NoZ) == 0);
		
		if (DragRange(labelZ, dataType, mZ, speed, min, max, format))
		{
			Update(TheVectorPool().Get(handle));
			valueChanged = true;
		}
	}

	return valueChanged;
}

void CSegmentQuadratic::CalcQuad(ERangeType type, float& a, float& b, float& c)
{
	a = (2.0F * mEnd[type] - 4.0F * mMid[type]) + 2.0F * mStart[type];
	b = (4.0F * mMid[type] - 3.0F * mStart[type]) - mEnd[type];
	c = mStart[type];
}

void CSegmentQuadratic::CalcPoints(float a, float b, float c, float& start, float& mid, float& end)
{
	start = c;
	mid = (0.25F * a) + (0.5F * b) + c;
	end = a + b + c;
}

CSegmentQuadratic::CSegmentQuadratic()
{	
	mTransitionType = TRANSITION_CONSTANT;
	
	mStart[RANGE_MIN] = 0.0F;
	mEnd[RANGE_MIN] = 0.0F;
	mMid[RANGE_MIN] = 0.0F;
	mClamp[RANGE_MIN] = -10000.0F;

	mStart[RANGE_MAX] = 0.0F;
	mEnd[RANGE_MAX] = 0.0F;
	mMid[RANGE_MAX] = 0.0F;
	mClamp[RANGE_MAX] = 10000.0F;
}

ETransitionType GetTransitionFromQuad(float a, float b, float c, float epsilon = 0.001F)
{
	if (std::abs(a) <= epsilon && std::abs(b) <= epsilon)
	{
		return TRANSITION_CONSTANT;
	}
	else if (std::abs(a) <= epsilon)
	{
		return TRANSITION_LINEAR;
	}
	else
	{
		return TRANSITION_QUADRATIC;
	}
}

void CSegmentQuadratic::Set(const CRangedQuadratic& quad)
{
	float minA = quad._base._a;
	float minB = quad._base._b;
	float minC = quad._base._c;

	float maxA = minA + quad._range[0];
	float maxB = minB + quad._range[1];
	float maxC = minC + quad._range[2];

	ETransitionType minT = GetTransitionFromQuad(minA, minB, minC);
	ETransitionType maxT = GetTransitionFromQuad(maxA, maxB, maxC);

	mTransitionType = minT > maxT ? minT : maxT;

	CalcPoints(minA, minB, minC, mStart[RANGE_MIN], mMid[RANGE_MIN], mEnd[RANGE_MIN]);
	mClamp[RANGE_MIN] = quad._base._min;

	CalcPoints(maxA, maxB, maxC, mStart[RANGE_MAX], mMid[RANGE_MAX], mEnd[RANGE_MAX]);
	mClamp[RANGE_MAX] = quad._base._max;
}

void CSegmentQuadratic::Set(int handle)
{
	Set(TheQuadPool().Get(handle));
}

void CSegmentQuadratic::Update(CRangedQuadratic& quad)
{
	switch (mTransitionType)
	{
	case TRANSITION_CONSTANT:
		quad._base._a = 0.0F;
		quad._base._b = 0.0F;
		quad._base._c = mStart[RANGE_MIN];
		quad._range[0] = 0.0F;
		quad._range[1] = 0.0F;
		quad._range[2] = mStart[RANGE_MAX] - mStart[RANGE_MIN];
		break;
	case TRANSITION_LINEAR:
		quad._base._a = 0.0F;
		quad._base._b = mEnd[RANGE_MIN] - mStart[RANGE_MIN];
		quad._base._c = mStart[RANGE_MIN];
		quad._range[0] = 0.0F;
		quad._range[1] = (mEnd[RANGE_MAX] - mEnd[RANGE_MIN]) - (mStart[RANGE_MAX] - mStart[RANGE_MIN]);
		quad._range[2] = mStart[RANGE_MAX] - mStart[RANGE_MIN];
		break;
	case TRANSITION_QUADRATIC:
		float minA, minB, minC;
		float maxA, maxB, maxC;

		CalcQuad(RANGE_MIN, minA, minB, minC);
		CalcQuad(RANGE_MAX, maxA, maxB, maxC);
		quad.Set(minA, minB, minC, maxA, maxB, maxC, mClamp[RANGE_MAX], mClamp[RANGE_MIN]);
		break;
	}
}

bool TransitionCombo(int& type)
{
	static const char* transitions[TRANSITION_MAX] =
	{
		"Constant",
		"Linear",
		"Quadratic",
	};

	if (ImGui::BeginCombo("Transition", transitions[type]))
	{
		bool changed = false;

		for (int i = 0; i < TRANSITION_MAX; i++)
		{
			const char* label = transitions[i];
			bool selected = i == type;

			if (ImGui::Selectable(label, selected))
			{
				type = i;
				changed = true;
			}

			if (selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
		return changed;
	}

	return false;
}

bool CSegmentQuadratic::Draw(int handle, float speed, float min, float max, const char* format, SegmentQuadraticFlags flags)
{	
	EDragDataType dataType = (flags & SegmentQuadraticFlags_Angle) != 0 ? DDT_ANGLE : DDT_FLOAT;
	bool valueChanged = false;
	
	{
		GroupSection section((flags & SegmentQuadraticFlags_NoStart) == 0);
		
		if (DragRange("Start", dataType, mStart, speed, min, max, format))
		{
			Update(TheQuadPool().Get(handle));
			valueChanged = true;
		}
	}

	{
		GroupSection section(mTransitionType == TRANSITION_QUADRATIC && (flags & SegmentQuadraticFlags_NoMid) == 0);

		if (DragRange("Mid", dataType, mMid, speed, min, max, format))
		{
			Update(TheQuadPool().Get(handle));
			valueChanged = true;
		}
	}

	{
		GroupSection section(mTransitionType != TRANSITION_CONSTANT && (flags & SegmentQuadraticFlags_NoEnd) == 0);
		
		if (DragRange("End", dataType, mEnd, speed, min, max, format))
		{
			Update(TheQuadPool().Get(handle));
			valueChanged = true;
		}
	}

	if (TransitionCombo(mTransitionType))
	{
		Update(TheQuadPool().Get(handle));
		valueChanged = true;
	}

	{
		GroupSection section((flags & SegmentQuadraticFlags_NoClamp) == 0);
		
		if (DragRange("Clamp", dataType, mClamp, speed, 0.0F, 0.0F, format))
		{
			CRangedQuadratic& quad = TheQuadPool().Get(handle);
			quad._base._min = mClamp[RANGE_MIN];
			quad._base._max = mClamp[RANGE_MAX];
			valueChanged = true;
		}
	}

	return valueChanged;
}

CSegmentColor::CSegmentColor()
{
	mTransitionType = TRANSITION_CONSTANT;

	mStart[RANGE_MIN] = igVec4f::OneVector;
	mEnd[RANGE_MIN] = igVec4f::OneVector;
	mMid[RANGE_MIN] = igVec4f::OneVector;

	mStart[RANGE_MAX] = igVec4f::OneVector;
	mEnd[RANGE_MAX] = igVec4f::OneVector;
	mMid[RANGE_MAX] = igVec4f::OneVector;
}

ETransitionType GetTransitionFromColor(CFxPrimitiveTemplate* primitive)
{
	ETransitionType transition = TRANSITION_CONSTANT;

	for (int i = 0; i < 3; i++)
	{
		CFxPrimitiveTemplate::EColorType type = CFxPrimitiveTemplate::EColorType(i);

		float startMin = primitive->UnPackColor(primitive->field_100, type) / 255.0F;
		float midMin = primitive->UnPackColor(primitive->field_104, type) / 255.0F;
		float endMin = primitive->UnPackColor(primitive->field_108, type) / 255.0F;

		float startMax = primitive->UnPackColor(primitive->field_112, type) / 255.0F;
		float midMax = primitive->UnPackColor(primitive->field_116, type) / 255.0F;
		float endMax = primitive->UnPackColor(primitive->field_120, type) / 255.0F;

		CQuadratic minQuad, maxQuad;
		primitive->CalcColorQuadratic(&minQuad, startMin, midMin, endMin);
		primitive->CalcColorQuadratic(&maxQuad, startMax, midMax, endMax);

		ETransitionType minT = GetTransitionFromQuad(minQuad._a, minQuad._b, minQuad._c);
		ETransitionType maxT = GetTransitionFromQuad(maxQuad._a, maxQuad._b, maxQuad._c);

		transition = std::max({ transition, minT, maxT });
	}

	return transition;
}

void CSegmentColor::Set(CFxPrimitiveTemplate* primitive)
{
	mTransitionType = GetTransitionFromColor(primitive);

	mStart[RANGE_MIN].unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, primitive->field_100);
	mMid[RANGE_MIN].unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, primitive->field_104);
	mEnd[RANGE_MIN].unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, primitive->field_108);

	mStart[RANGE_MAX].unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, primitive->field_112);
	mMid[RANGE_MAX].unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, primitive->field_116);
	mEnd[RANGE_MAX].unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, primitive->field_120);
}

void CSegmentColor::Update(CFxPrimitiveTemplate* primitive, ERangeType range)
{	
	igUnsignedInt start = mStart[range].packColor(IG_MATH_COLORSPACE_FORMAT_RGBA);
	igUnsignedInt mid = start;
	igUnsignedInt end = start;

	if (mTransitionType != TRANSITION_CONSTANT)
	{
		end = mEnd[range].packColor(IG_MATH_COLORSPACE_FORMAT_RGBA);

		if (mTransitionType == TRANSITION_QUADRATIC)
		{
			mid = mMid[range].packColor(IG_MATH_COLORSPACE_FORMAT_RGBA);
		}
		else
		{
			igVec4f midColor = mStart[range] + mEnd[range];

			midColor *= 0.5F;
			mid = midColor.packColor(IG_MATH_COLORSPACE_FORMAT_RGBA);
		}
	}

	if (range == RANGE_MIN)
	{
		primitive->field_100 = start;
		primitive->field_104 = mid;
		primitive->field_108 = end;
	}
	else
	{
		primitive->field_112 = start;
		primitive->field_116 = mid;
		primitive->field_120 = end;
	}
}

bool CSegmentColor::Draw(CFxPrimitiveTemplate* primitive)
{
	ImGuiColorEditFlags minFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha;
	ImGuiColorEditFlags maxFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha;
	
	bool valueChanged = false;

	if (ImGui::ColorEdit4("##StartMin", mStart[RANGE_MIN].vec, minFlags))
	{
		Update(primitive, RANGE_MIN);
		valueChanged = true;
	}
	
	ImGui::SameLine();

	if (ImGui::ColorEdit4("Start##StartMax", mStart[RANGE_MAX].vec, maxFlags))
	{
		Update(primitive, RANGE_MAX);
		valueChanged = true;
	}

	{
		GroupSection section(mTransitionType == TRANSITION_QUADRATIC);

		if (ImGui::ColorEdit4("##MidMin", mMid[RANGE_MIN].vec, minFlags))
		{
			Update(primitive, RANGE_MIN);
			valueChanged = true;
		}

		ImGui::SameLine();

		if (ImGui::ColorEdit4("Mid##MidMax", mMid[RANGE_MAX].vec, maxFlags))
		{
			Update(primitive, RANGE_MAX);
			valueChanged = true;
		}
	}

	{
		GroupSection section(mTransitionType != TRANSITION_CONSTANT);

		if (ImGui::ColorEdit4("##EndMin", mEnd[RANGE_MIN].vec, minFlags))
		{
			Update(primitive, RANGE_MIN);
			valueChanged = true;
		}

		ImGui::SameLine();

		if (ImGui::ColorEdit4("End##EndMax", mEnd[RANGE_MAX].vec, maxFlags))
		{
			Update(primitive, RANGE_MAX);
			valueChanged = true;
		}
	}

	if (TransitionCombo(mTransitionType))
	{
		Update(primitive, RANGE_MIN);
		Update(primitive, RANGE_MAX);
		valueChanged = true;
	}

	return valueChanged;
}
