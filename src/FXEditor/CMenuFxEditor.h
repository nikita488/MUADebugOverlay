#pragma once

#include <UI/CMenu.h>
#include <Scene/IModel.h>

class CMenuItem;

class CMenuFxEditor : public CMenu
{
private:
	CMenuItem* mGrid;
	CMenuItem* mAxis;

public:
	CMenuFxEditor();
	virtual ~CMenuFxEditor();

	virtual bool Initialize(TiXmlElement* menuElement, TiXmlElement* genericNode);

	virtual void HandleInput();

	virtual EMenuExec OnCancel();

	virtual void Draw();
};

