#pragma once

#include "IDebugWindow.h"
#include "COverlayWindow.h"

class CMiscWindow : public IDebugWindow
{
private:
	COverlayWindow* mOverlay;
	int mTeamRepAmount;
	int mXPAmount;
	int mXPAllAmount;
	int mMoneyAmount;

public:
	CMiscWindow();

	virtual void Draw();

	inline void SetOverlay(COverlayWindow* overlay)
	{
		mOverlay = overlay;
	}
};
