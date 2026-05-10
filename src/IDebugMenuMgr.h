#pragma once

class IDebugMenuMgr
{
public:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void RunFrame() = 0;
	virtual void Draw() = 0;
	virtual bool GetEnabled() const = 0;
	virtual void SetEnabled(bool enabled) = 0;
	virtual bool WantCaptureMouse() const = 0;
	virtual bool WantCaptureKeyboard() const = 0;
};

extern IDebugMenuMgr& TheDebugMenuMgr();
