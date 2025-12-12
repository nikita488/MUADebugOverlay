#pragma once

#include "IDebugWindow.h"
#include "COverlayWindow.h"

class CMiscWindow : public IDebugWindow
{
private:
	COverlayWindow* mOverlay;

public:
	virtual void Draw();

	inline void SetOverlay(COverlayWindow* overlay)
	{
		mOverlay = overlay;
	}
};
