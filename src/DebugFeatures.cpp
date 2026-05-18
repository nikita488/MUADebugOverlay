#include "DebugFeatures.h"

#include <MUASDK.h>
#include <Utils/MemoryMgr.h>
#include <safetyhook/safetyhook.hpp>

#include <Misc/Common.h>
#include <Misc/ICommand.h>
#include <Misc/DebugGraphics.h>
#include <Game/IDeveloperInput.h>
#include <Display/IDisplay.h>
#include <Scene/IXmlCache.h>
#include <UI/IUICommands.h>

#include <UI/CMenuDebug.h>

#include <Game/CGame.h>
#include <Game/CActor.h>
#include <Game/ICamera.h>
#include <Game/IEntityVisibility.h>
#include <Game/CSmartEnt.h>

static bool ConfigSystemInFinal()
{
	return true;
}

static void VersionDataHandler(CVarData* data)
{
	ratl::string_vs<256> version;
	const char* translation = TheXmlCache().GetTranslation();
	const char* mode = TheDisplay().GetModeName();

	FormatToStringBuffer(version.c_str(), 256, "%s %s %s %s", "Sep 22 2006", "15:23:44", mode, translation);
	data->SetString(version.c_str());
}

__declspec(naked) void ActorMoveEpilogue()
{
	static const DWORD epilogue = 0x46B2D4;

	_asm
	{
		pop ebp
		jmp epilogue
	}
}

void ActorMoveHook(SafetyHookContext& ctx)
{
	CActor* actor = (CActor*)ctx.esi;
	TActorMove* am = (TActorMove*)ctx.ebx;

	if (actor->mDebugMove)
	{
		actor->AM_DebugMove(am);
		ctx.eip = uintptr_t(&ActorMoveEpilogue);
	}
}

void FreeViewHook(SafetyHookContext& ctx)
{
	CGame* game = (CGame*)ctx.esi;
	
	if (TheCamera().CameraIsInFreeView() && TheCamera().GetFreeViewPaused())
	{
		igVec3f cameraPos;

		game->ControllerInput();
		game->CameraSet();

		igMatrix44f viewMatrix;
		TheCamera().GetCameraMatrix(viewMatrix);
		viewMatrix.getTranslation(cameraPos);

		game->Playfield()->SetCameraViewMatrix(viewMatrix);
		game->Playfield()->SetCameraFOV(TheCamera().GetCameraAspectFOV());
		game->Playfield()->SetLightPickerSpot(TheCamera().GetFocusPoint());

		const float dist = 100.0F;
		TheSmartEntMgr().TestSmartEnts(cameraPos.distance2(game->mOldCameraPos) > (dist * dist));
		game->mOldCameraPos = cameraPos;
		TheEntityVisibility().UpdateVisibility(viewMatrix);

		ctx.eip = 0x4B54E7;
	}
}

DebugFeatures::DebugFeatures()
{
	using namespace MUASDK;
	using namespace Memory::VP;
	
	WriteMemDisplacement(0x4B0CAB + 1, VersionDataHandler);

	InjectHook(0x4AD5DA, TheDeveloperInput);
	InjectHook(0x4B0BEE, TheDeveloperInput);
	InjectHook(0x4B5503, TheDeveloperInput);

	WriteMemDisplacement(0x7C02DC, ConfigSystemInFinal);

	static SafetyHookMid freeViewHook = safetyhook::create_mid(0x4B4CEC, FreeViewHook);
	static SafetyHookMid debugMoveHook = safetyhook::create_mid(0x46B19B, ActorMoveHook);

	OnGameInitEvent() += []()
	{
		InitDebugGraphics();
	};

	OnGameShutdownEvent() += []()
	{
		ShutdownDebugGraphics();
	};

	OnUICommandsInitEvent() += []()
	{
		Command().RegisterCommand("loadpoint", LoadPointFunc);
		Command().RegisterCommand("loadmapaddteam", LoadMapAddTeamFunc);
		Command().RegisterCommand("noclip", NoClipFunc);
		Command().RegisterCommand("freeview", FreeViewFunc);
		Command().RegisterCommand("give", GiveFunc);
		Command().RegisterCommand("slowmo", SlowMotionFunc);
		Command().RegisterCommand("reloadzonenopersistence", ReloadZoneNoPersistenceFunc);
	};

	RegisterMenu<CMenuDebug>("DEBUG_MENU");
}

DebugFeatures plugin;
