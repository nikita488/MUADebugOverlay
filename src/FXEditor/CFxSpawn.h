#pragma once

#include <igGapMath.h>
#include <igMath/igVec3f.h>

using namespace Gap;

class CFxSpawn
{
private:
	static float NO_LIFE_OVERRIDE;
	static igVec3f NO_COLOR_OVERRIDE;

	bool mVisible;
	
	igVec3f mOrigin;
	igVec3f mForwardAxis;
	int mLevel;
	float mLife;
	float mTimeScale;
	float mSizeScale;
	igVec3f mColor;

	bool mOverrideLife;
	bool mOverrideColor;

public:
	CFxSpawn();

	void Reset();

	void Update();

	inline void SetVisible(bool visible)
	{
		mVisible = visible;
	}

	inline bool IsVisible() const
	{
		return mVisible;
	}

	inline const igVec3f& GetOrigin() const
	{
		return mOrigin;
	}

	inline const igVec3f& GetForwardAxis() const
	{
		return mForwardAxis;
	}

	inline int GetFxLevel() const
	{
		return mLevel;
	}

	inline float GetLife() const
	{
		return mOverrideLife ? mLife : NO_LIFE_OVERRIDE;
	}

	inline float GetTimeScale() const
	{
		return mTimeScale;
	}

	inline float GetSizeScale() const
	{
		return mSizeScale;
	}

	inline const igVec3f& GetColor() const
	{
		return mOverrideColor ? mColor : NO_COLOR_OVERRIDE;
	}
};
