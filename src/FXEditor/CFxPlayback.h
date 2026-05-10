#pragma once

#include <igGapMath.h>
#include <igMath/igVec3f.h>

using namespace Gap;

class CFxEditor;
class CFxTemplate;

enum ERepeatMode
{
	REPEAT_ONCE,
	REPEAT_UNTIL_STOPPED,
	REPEAT_FOR_DURATION,
	REPEAT_PERSISTENT,
};

class CFxPlayback
{
private:
	bool mVisible;

	int mRepeatMode;
	float mPlayDuration;

	bool mRespawnEveryFrame;
	float mRepeatRate;
	float mRandomTime;

	bool mAnimateSpawnPoint;
	igVec3f mSpawnPointVelocity;
	float mSpawnPointResetTime;

	void RepeatModeGroup();
	void RepeatRateGroup();
	void SpawnPointMovementGroup();

public:
	CFxPlayback();

	void Initialize(CFxTemplate* fxTemplate);

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

	inline ERepeatMode GetRepeatMode() const
	{
		return ERepeatMode(mRepeatMode);
	}

	inline float GetPlayDuration() const
	{
		return mPlayDuration;
	}

	inline bool IsRespawnEveryFrame() const
	{
		return mRespawnEveryFrame;
	}

	inline bool IsAnimateSpawnPoint() const
	{
		return mAnimateSpawnPoint;
	}

	inline const igVec3f& GetSpawnPointVelocity() const
	{
		return mSpawnPointVelocity;
	}

	inline float GetSpawnPointResetTime() const
	{
		return mSpawnPointResetTime;
	}
};

