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
#include <UI/CMenuCodex.h>//TODO: Move to MUAPatch

#include <Game/CGame.h>
#include <Game/CActor.h>
#include <Game/ICamera.h>
#include <Game/IEntityVisibility.h>
#include <Game/CSmartEnt.h>

#include <Misc/CConfig.h>
#include <Input/IInput.h>
#include <Game/IMultiplayer.h>
#include <Game/ISimulator.h>

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

//void FreeViewHook(SafetyHookContext& ctx)
//{
//	CGame* game = (CGame*)ctx.esi;
//	
//	if (TheCamera().CameraIsInFreeView() && TheCamera().GetFreeViewPaused())
//	{
//		igVec3f cameraPos;
//
//		game->ControllerInput();
//		game->CameraSet();
//
//		igMatrix44f viewMatrix;
//		TheCamera().GetCameraMatrix(viewMatrix);
//		viewMatrix.getTranslation(cameraPos);
//
//		game->Playfield()->SetCameraViewMatrix(viewMatrix);
//		game->Playfield()->SetCameraFOV(TheCamera().GetCameraAspectFOV());
//		game->Playfield()->SetLightPickerSpot(TheCamera().GetFocusPoint());
//
//		const float dist = 100.0F;
//		TheSmartEntMgr().TestSmartEnts(cameraPos.distance2(game->mOldCameraPos) > (dist * dist));
//		game->mOldCameraPos = cameraPos;
//		TheEntityVisibility().UpdateVisibility(viewMatrix);
//
//		ctx.eip = 0x4B54E7;
//	}
//}

SafetyHookInline g_freeViewHook;

static void __fastcall FreeViewHook(CGame* game)
{
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
	}
	else
	{
		g_freeViewHook.thiscall(game);
	}
}

void LaunchOptionsHook(SafetyHookContext& ctx)
{
	bool launchMap = false;
	XMenIni().Get("INIT", "launchMap", &launchMap);

	Command().ExecuteCommandString("resetgame");

	bool launchSimulator = false;
	XMenIni().Get("SIMULATOR", "launchSimulator", &launchSimulator);

	ratl::string_vs<64> pauseMenu("pda");
	XMenIni().Get("INIT", "pauseMenu", pauseMenu.c_str(), pauseMenu.capacity(), "pda");

	TheMenuMgr().SetPauseMenu(pauseMenu.c_str());

	ratl::string_vs<64> teamMenu("team");
	XMenIni().Get("INIT", "teamMenu", teamMenu.c_str(), teamMenu.capacity(), "team");

	TheMenuMgr().SetTeamMenu(teamMenu.c_str());

	if (launchMap)
	{
		EPlayerId playerId = TheInput().GetNthConnectedPlayer(1);

		if (playerId == EPLAYERID_NONE)
		{
			playerId = EPLAYERID_ONE;
		}

		TheMultiplayerSystem().SetFirstPrimaryPlayerId(playerId);

		TheMenuMgr().SetIsReadyForUse();//Bugfix for pause menu not opening

		ratl::string_vs<64> mapName;

		Command().ExecuteCommandString("resetgame");

		ratl::string_vs<100> command;

		XMenIni().Get("MAP", "filename", mapName.c_str(), mapName.capacity());

		ratl::string_vs<64> fixedPath(Filename_EnsureForwardSlashes(mapName.c_str()));

		ratl::str::printf(command.c_str(), "loadmapaddteam %s", fixedPath.c_str());
		Command().ExecuteCommandString(command.c_str());

		ctx.eip = 0x418950;
	}
	else if (launchSimulator)
	{
		EPlayerId playerId = TheInput().GetNthConnectedPlayer(1);

		if (playerId == EPLAYERID_NONE)
		{
			playerId = EPLAYERID_ONE;
		}

		TheMultiplayerSystem().SetFirstPrimaryPlayerId(playerId);

		TheMenuMgr().SetIsReadyForUse();//Bugfix for pause menu not opening

		Command().ExecuteCommandString("resetgame");

		ratl::string_vs<100> command;

		int courseIndex;
		XMenIni().Get("SIMULATOR", "courseIndex", &courseIndex);

		if (courseIndex >= 0)
		{
			TheSimulator().func_60(courseIndex, false, false);
		}

		ctx.eip = 0x418950;
	}
	else
	{
		ratl::string_vs<64> startMenu(TheMenuMgr().GetStartMenu());

		XMenIni().Get("INIT", "startMenu", startMenu.c_str(), startMenu.capacity(), TheMenuMgr().GetStartMenu());

		if (stricmp(startMenu.c_str(), TheMenuMgr().GetStartMenu()) == 0)
		{
			ctx.eip = 0x41887A;
		}
		else
		{
			EPlayerId playerId = TheInput().GetNthConnectedPlayer(1);

			if (playerId == EPLAYERID_NONE)
			{
				playerId = EPLAYERID_ONE;
			}

			TheMultiplayerSystem().SetFirstPrimaryPlayerId(playerId);
			TheMultiplayerSystem().SetRealPrimaryController(EController(playerId));

			TheMenuMgr().OpenMenu(startMenu.c_str(), NULL);

			ctx.eip = 0x418950;
		}
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

	g_freeViewHook = safetyhook::create_inline(0x4B4CE0, FreeViewHook);

	//static SafetyHookMid freeViewHook = safetyhook::create_mid(0x4B4CEC, FreeViewHook);
	static SafetyHookMid debugMoveHook = safetyhook::create_mid(0x46B19B, ActorMoveHook);
	static SafetyHookMid launchOptionsHook = safetyhook::create_mid(0x4187E1, LaunchOptionsHook);

	OnClientPreInitEvent() += []()
	{
		RegisterMenu<CMenuDebug>("DEBUG_MENU");
		RegisterMenu<CMenuCodex>("CODEX_MENU");
	};

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
}

DebugFeatures plugin;
