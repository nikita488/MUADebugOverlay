#pragma once

#include "../IDebugWindow.h"

extern bool g_gammaEnabled;

class CFxEditorWindow : public IDebugWindow
{
public:
	virtual void Draw();
};

