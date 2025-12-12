#pragma once

#include "IDebugWindow.h"

#include <imgui/imgui.h>

class COverlayWindow : public IDebugWindow
{
private:
	ImVec4 mEntityInfoColor;
	float mEntityInfoRange;
	bool mShowEntityName : 1;
	bool mShowEntityPos : 1;
	bool mShowEntityAngle : 1;

public:
	COverlayWindow();

	virtual void Draw();

	inline const ImVec4& GetEntityInfoColor() const
	{
		return mEntityInfoColor;
	}

	inline void SetEntityInfoColor(const ImVec4& color)
	{
		mEntityInfoColor = color;
	}

	inline float GetEntityInfoRange() const
	{
		return mEntityInfoRange;
	}

	inline void SetEntityInfoRange(float range)
	{
		mEntityInfoRange = range;
	}

	inline bool GetShowEntityName() const
	{
		return mShowEntityName;
	}

	inline void SetShowEntityName(bool show)
	{
		mShowEntityName = show;
	}

	inline bool GetShowEntityPos() const
	{
		return mShowEntityPos;
	}

	inline void SetShowEntityPos(bool show)
	{
		mShowEntityPos = show;
	}

	inline bool GetShowEntityAngle() const
	{
		return mShowEntityAngle;
	}

	inline void SetShowEntityAngle(bool show)
	{
		mShowEntityAngle = show;
	}
};

