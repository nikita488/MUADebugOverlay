#pragma once

class CFxPrimitiveTemplate;

enum EBolt
{
	BOLT_1,
	BOLT_2,
};

class CBoltSettings
{
private:
	EBolt mBolt;
	bool mEnabled;
	bool mBoltOntoEntFxOrigin;
	bool mUnboltImmediately;
	bool mUnboltUpAxis;
	bool mUnboltGradually;

public:
	CBoltSettings(EBolt bolt);

	void Set(CFxPrimitiveTemplate* primTemplate);

	bool Update(CFxPrimitiveTemplate* primTemplate);
};
