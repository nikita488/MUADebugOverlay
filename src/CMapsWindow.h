#pragma once

#include "IDebugWindow.h"

#include <vector>
#include <string>

class CMapsWindow : public IDebugWindow
{
private:
	std::vector<std::string> mLoadPoints;
	int mSelectedLoadpoint;
	int mSelectedZone;

public:
	CMapsWindow();

	void LoadPointList();

	virtual void Initialize();
	virtual void Shutdown();

	void LoadSelectedLoadPoint();

	void DrawLoadPointList();

	void LoadSelectedZone();

	void DrawZoneList();

	virtual void Draw();
};

