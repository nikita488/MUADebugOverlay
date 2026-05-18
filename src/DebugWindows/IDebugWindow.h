#pragma once

class IDebugWindow
{
public:
	virtual void Initialize() {};
	virtual void Shutdown() {};
	virtual void Draw() = 0;
};
