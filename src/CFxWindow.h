#pragma once

#include "IDebugWindow.h"

class CFxWindow : public IDebugWindow
{
public:
	static const char* fxPrimitiveNames[16];
	static const char* fxBlendNames[5];

public:
	virtual void Draw();
};

