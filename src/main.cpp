#include <Windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <Utils/MemoryMgr.h>
#include <safetyhook/safetyhook.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>

#include <Game/CGame.h>
#include <Game/IDeveloperInput.h>
#include <Game/CScriptTriggerEntity.h>
#include <Game/CActor.h>
#include <Game/ICamera.h>

#include <UI/CUICommands.h>
#include <UI/CMenuFactory.h>
#include <UI/CMenuDebug.h>
#include <UI/CMenuCodex.h>

#include <Display/IDisplay.h>
#include <Scene/IXmlCache.h>

#include <Misc/ICommand.h>
#include <Misc/DebugGraphics.h>

#include "MUASDK.h"
#include "IDebugMenuMgr.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
		| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCTSTR)GetCurrentModule, &hModule);
	return hModule;
}

int GetBaseAddress()
{
	static int addr = reinterpret_cast<int>(GetModuleHandleA(NULL));
	return addr;
}

const int GetExternalAddress(const char* processName, int shift, int address)
{
	static int addr = reinterpret_cast<int>(GetModuleHandleA(processName));
	return (GetBaseAddress() - 0x400000) + (addr - shift + address);
}

static void HandleImguiInputs(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	if (!ImGui::GetCurrentContext())
		return;

	if (msg == WM_KEYDOWN && wParam == VK_F6)
	{
		TheDebugMenuMgr().SetEnabled(!TheDebugMenuMgr().GetEnabled());
		ImGui::GetIO().MouseDrawCursor = TheDebugMenuMgr().GetEnabled();
	}

	if (TheDebugMenuMgr().GetEnabled())
	{
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
	}
}

static LRESULT (*orgsub_71EB20)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing);

static LRESULT sub_71EB20Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	HandleImguiInputs(hwnd, msg, wParam, lParam, keepProcessing);
	return orgsub_71EB20(hwnd, msg, wParam, lParam, keepProcessing);
}

static LRESULT (*orgsub_71EB70)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing);

static LRESULT sub_71EB70Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	HandleImguiInputs(hwnd, msg, wParam, lParam, keepProcessing);
	return orgsub_71EB70(hwnd, msg, wParam, lParam, keepProcessing);
}

static HRESULT (WINAPI *orgGetDeviceState)(LPDIRECTINPUTDEVICE8 device, DWORD cbData, LPVOID lpvData);

static HRESULT WINAPI GetDeviceStateHook(LPDIRECTINPUTDEVICE8 device, DWORD cbData, LPVOID lpvData)
{
	if (TheDebugMenuMgr().GetEnabled())
	{
		if ((cbData == 256 && TheDebugMenuMgr().WantCaptureKeyboard()) || (cbData == sizeof(DIMOUSESTATE2) && TheDebugMenuMgr().WantCaptureMouse()))
		{
			ZeroMemory(lpvData, cbData);
			return DI_OK;
		}
	}

	return orgGetDeviceState(device, cbData, lpvData);
}

static HRESULT (WINAPI *orgGetDeviceData)(LPDIRECTINPUTDEVICE8 device, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);

static HRESULT WINAPI GetDeviceDataHook(LPDIRECTINPUTDEVICE8 device, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	if (TheDebugMenuMgr().WantCaptureMouse())
	{
		*pdwInOut = 0;
		return DI_OK;
	}

	return orgGetDeviceData(device, cbObjectData, rgdod, pdwInOut, dwFlags);
}

static void (__thiscall *orgReleaseVolatileResources)(void* vc);

static void __fastcall ReleaseVolatileResourcesHook(void* vc)
{
	orgReleaseVolatileResources(vc);
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

static void (__thiscall *orgRestoreVolatileResources)(void* vc);

static void __fastcall RestoreVolatileResourcesHook(void* vc)
{
	orgRestoreVolatileResources(vc);
	ImGui_ImplDX9_CreateDeviceObjects();
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
	if (TheCamera().CameraIsInFreeView() && TheCamera().GetFreeViewPaused())
	{
		((CGame*)ctx.esi)->FreeViewUpdate();
		ctx.eip = 0x4B54E7;
	}
}

void DrawTriggersDrawHook(SafetyHookContext& ctx)
{
	((CScriptTriggerEntity*)ctx.esi)->Draw();
}

void DrawTriggersCollideHook(SafetyHookContext& ctx)
{
	if (true)
	{
		igMatrix44f& mat = *(igMatrix44f*)ctx.ebx;
		DebugLocalBox(gDLCyan, mat, igVec3f(-1.0F, -1.0F, -1.0F), igVec3f(1.0F, 1.0F, 1.0F), 0.5F);
	}
}

//void MenuFactoryCreateHook(SafetyHookContext& ctx)
//{
//	if (!ctx.eax)
//	{
//		return;
//	}
//
//	const char* type = (const char*)ctx.esi;
//
//	if (!stricmp(type, "CODEX_MENU"))
//	{
//		ctx.eax = uintptr_t(new (menuFactoryMenus.alloc_value_raw()) CMenuCodex());
//		ctx.eip = 0x6BB87B;
//	}
//	//else if (!stricmp(type, "DEBUG_MENU"))
//	//{
//	//	ctx.eax = uintptr_t(new (menuFactoryMenus.alloc_value_raw()) CMenuDebug());
//	//	ctx.eip = 0x6BB87B;
//	//}
//	else
//	{
//		ctx.eip = 0x6BB881;
//	}
//}

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

void OnInitializeHook()
{
	using namespace MUASDK;
	using namespace Memory::VP;

#if _DEBUG
	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
#endif

	InstallHooks();

	HRESULT result;
	LPDIRECTINPUT8 input = NULL;

	if ((result = DirectInput8Create(GetCurrentModule(), DIRECTINPUT_VERSION, IID_IDirectInput8W, (LPVOID*)&input, NULL)) != DI_OK)
	{
		printf("Failed to create DirectInput object. Error code: %d\n", result);
		return;
	}

	LPDIRECTINPUTDEVICE8 inputDevice = NULL;

	if ((result = input->CreateDevice(GUID_SysMouse, &inputDevice, NULL)) != DI_OK)
	{
		printf("Failed to create DirectInputDevice object. Error code: %d\n", result);
		input->Release();
		return;
	}

	intptr_t pDirectInputDeviceVTable = *reinterpret_cast<intptr_t*>(inputDevice);
	intptr_t pGetDeviceState = pDirectInputDeviceVTable + sizeof(intptr_t) * 9;
	intptr_t pGetDeviceData = pDirectInputDeviceVTable + sizeof(intptr_t) * 10;

	InterceptMemDisplacement(pGetDeviceState, orgGetDeviceState, GetDeviceStateHook);
	InterceptMemDisplacement(pGetDeviceData, orgGetDeviceData, GetDeviceDataHook);

	inputDevice->Release();
	input->Release();

	//static SafetyHookMid drawTriggers1Hook = safetyhook::create_mid(0x5246B1, DrawTriggersDrawHook);
	//static SafetyHookMid drawTriggers2Hook = safetyhook::create_mid(0x52461B, DrawTriggersDrawHook);
	//static SafetyHookMid drawTriggers3Hook = safetyhook::create_mid(0x487B06, DrawTriggersCollideHook);
	//static SafetyHookMid menuFactoryCreateHook = safetyhook::create_mid(0x6BB85E, MenuFactoryCreateHook);
	static SafetyHookMid freeViewHook = safetyhook::create_mid(0x4B4CEC, FreeViewHook);
	static SafetyHookMid debugMoveHook = safetyhook::create_mid(0x46B19B, ActorMoveHook);

	InterceptCall(GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3B3), orgReleaseVolatileResources, ReleaseVolatileResourcesHook);
	InterceptCall(GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3D5), orgRestoreVolatileResources, RestoreVolatileResourcesHook);
	
	InterceptMemDisplacement(0x720C8C + 1, orgsub_71EB20, sub_71EB20Hook);
	InterceptMemDisplacement(0x720CBD + 1, orgsub_71EB70, sub_71EB70Hook);

	WriteMemDisplacement(0x4B0CAB + 1, VersionDataHandler);

	InjectHook(0x4AD5DA, TheDeveloperInput);
	InjectHook(0x4B0BEE, TheDeveloperInput);
	InjectHook(0x4B5503, TheDeveloperInput);

	WriteMemDisplacement(0x7C02DC, ConfigSystemInFinal);

	OnInitEvent() += []()
	{
		TheDebugMenuMgr().Initialize();
	};

	OnShutdownEvent() += []()
	{
		TheDebugMenuMgr().Shutdown();
	};

	OnEndDrawEvent() += []()
	{
		TheDebugMenuMgr().Draw();
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
	};

	RegisterMenu<CMenuCodex>("CODEX_MENU");
	RegisterMenu<CMenuDebug>("DEBUG_MENU");
}