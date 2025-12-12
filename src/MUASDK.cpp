#include "MUASDK.h"

#include <Utils/MemoryMgr.h>
#include <safetyhook/safetyhook.hpp>

namespace MUASDK
{

Event<>& OnInitEvent()
{
	static Event<> InitEvent;
	return InitEvent;
}

Event<>& OnShutdownEvent()
{
	static Event<> ShutdownEvent;
	return ShutdownEvent;
}

Event<>& OnEndDrawEvent()
{
	static Event<> EndDrawEvent;
	return EndDrawEvent;
}

Event<>& OnGameInitEvent()
{
	static Event<> GameInitEvent;
	return GameInitEvent;
}

Event<>& OnGameShutdownEvent()
{
	static Event<> GameShutdownEvent;
	return GameShutdownEvent;
}

Event<>& OnGameRunFrameEvent()
{
	static Event<> GameRunFrameEvent;
	return GameRunFrameEvent;
}

Event<>& OnUICommandsInitEvent()
{
	static Event<> UICommandsInitEvent;
	return UICommandsInitEvent;
}

TMenuFactoryMap menuFactoryMap;

void MenuFactoryCreateHook(SafetyHookContext& ctx)
{
	if (!ctx.eax)
	{
		return;
	}

	const char* type = (const char*)ctx.esi;
	TMenuFactoryMapIt pair = menuFactoryMap.find(type);

	if (pair != menuFactoryMap.end())
	{
		ctx.eax = uintptr_t(pair->second());
		ctx.eip = 0x6BB87B;
	}
	else
	{
		ctx.eip = 0x6BB881;
	}
}

static void (__thiscall* orgClientInitialize)(void* client);

static void __fastcall ClientInitializeHook(void* client)
{
	orgClientInitialize(client);
	OnInitEvent().ExecuteAll();
}

static void (__thiscall* orgClientShutdown)(void* client);

static void __fastcall ClientShutdownHook(void* client)
{
	orgClientShutdown(client);
	OnShutdownEvent().ExecuteAll();
}

static bool (__thiscall* orgDisplayEndDraw)(void* display);

static bool __fastcall DisplayEndDrawHook(void* display)
{
	OnEndDrawEvent().ExecuteAll();
	return orgDisplayEndDraw(display);
}

static void (__thiscall* orgGameInitialize)(void* game);

static void __fastcall GameInitializeHook(void* game)
{
	orgGameInitialize(game);
	OnGameInitEvent().ExecuteAll();
}

static void (__thiscall* orgGameShutdown)(void* game);

static void __fastcall GameShutdownHook(void* game)
{
	orgGameShutdown(game);
	OnGameShutdownEvent().ExecuteAll();
}

static void (__thiscall* orgGameRunFrame)(void* game);

static void __fastcall GameRunFrameHook(void* game)
{
	orgGameRunFrame(game);
	OnGameRunFrameEvent().ExecuteAll();
}

static void(__thiscall* orgUICommandsInitialize)(void* uiCommands);

static void __fastcall UICommandsInitializeHook(void* uiCommands)
{
	orgUICommandsInitialize(uiCommands);
	OnUICommandsInitEvent().ExecuteAll();
}

void InstallHooks()
{
	using namespace Memory::VP;
	
	static SafetyHookMid menuFactoryCreateHook = safetyhook::create_mid(0x6BB85E, MenuFactoryCreateHook);
	
	InterceptMemDisplacement(0x7996C8, orgClientInitialize, ClientInitializeHook);
	InterceptMemDisplacement(0x7996CC, orgClientShutdown, ClientShutdownHook);
	InterceptMemDisplacement(0x7CD80C, orgDisplayEndDraw, DisplayEndDrawHook);

	InterceptMemDisplacement(0x7A2430, orgGameInitialize, GameInitializeHook);
	InterceptMemDisplacement(0x7A2434, orgGameShutdown, GameShutdownHook);
	InterceptMemDisplacement(0x7A2460, orgGameRunFrame, GameRunFrameHook);

	InterceptMemDisplacement(0x7CD1E4, orgUICommandsInitialize, UICommandsInitializeHook);
}

}
