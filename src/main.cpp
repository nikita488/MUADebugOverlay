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
#include <Misc/CConfig.h>

#include <MUASDK.h>

#include "IDebugMenuMgr.h"

#include "FXEditor/IFxEditor.h"
#include "FXEditor/CMenuFxEditor.h"
#include "FXEditor/CFxEditorWindow.h"

int WINAPI WinMainHook(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	igAlchemy alchemy;

	CMemory& memory = TheMemory();

	static IDTable* chainTable = (IDTable*)0x805860;

	for (int i = 0; i < 256; i++)
	{
		IDTable& table = chainTable[i];
		//printf("%s=%d\n", table.name, table.id);
		printf("%s=%d\n", table.name, table.id);
	}
	
	return 0;
}

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
	if (msg == WM_KEYDOWN && wParam == VK_F6)
	{
		TheDebugMenuMgr().SetEnabled(!TheDebugMenuMgr().GetEnabled());
	}

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
	//if (TheDebugMenuMgr().GetEnabled() || TheFxEditor().IsActive())
	//{
	//	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
	//}
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
	if (TheFxEditor().IsActive() || TheDebugMenuMgr().GetEnabled())
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
	if (TheFxEditor().IsActive() || TheDebugMenuMgr().WantCaptureMouse())
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

//void DrawTriggersDrawHook(SafetyHookContext& ctx)
//{
//	((CScriptTriggerEntity*)ctx.esi)->Draw();
//}
//
//void DrawTriggersCollideHook(SafetyHookContext& ctx)
//{
//	if (true)
//	{
//		igMatrix44f& mat = *(igMatrix44f*)ctx.ebx;
//		DebugLocalBox(gDLCyan, mat, igVec3f(-1.0F, -1.0F, -1.0F), igVec3f(1.0F, 1.0F, 1.0F), 0.5F);
//	}
//}
//
//#include <Common/Rascl/IRasclPreprocessor.h>
//
//igMatrix44f* traceMeshToWorld = NULL;
//
//void drawTri(Rascl::STriangle& tri)
//{
//	igVec3f v1;
//	igVec3f v2;
//	igVec3f v3;
//	igVec3f color(1.0F, 0.0F, 0.0F);
//
//	color[1] += ((tri.appFastFlags & 0xF) + 1) / 16.0F;
//	color[2] += (((tri.appFastFlags & 0xFF0) >> 4) + 1) / 32.0F;//probably 64?
//
//	if (false && traceMeshToWorld)
//	{
//		v1 = igVec3f(tri.v1[0], tri.v1[1], tri.v1[2]);
//		v1.transformPoint(v1, *traceMeshToWorld);
//
//		v2 = igVec3f(tri.v2[0], tri.v2[1], tri.v2[2]);
//		v2.transformPoint(v2, *traceMeshToWorld);
//
//		v3 = igVec3f(tri.v3[0], tri.v3[1], tri.v3[2]);
//		v3.transformPoint(v3, *traceMeshToWorld);
//	}
//	else
//	{
//		v1 = igVec3f(tri.v1[0], tri.v1[1], tri.v1[2]);
//		v2 = igVec3f(tri.v2[0], tri.v2[1], tri.v2[2]);
//		v3 = igVec3f(tri.v3[0], tri.v3[1], tri.v3[2]);
//	}
//
//	DebugLine(color, v1, v2, 0.5F);
//	DebugLine(color, v2, v3, 0.5F);
//	DebugLine(color, v3, v1, 0.5F);
//}
//
//void DrawCollisionHook(SafetyHookContext& ctx)
//{
//	Rascl::STriangle& tri = *reinterpret_cast<Rascl::STriangle*>(ctx.edi);
//	
//	if (true)
//	{
//		drawTri(tri);
//	}
//}

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

void SkipLegalScreenHook(SafetyHookContext& ctx)
{
	bool skipLegalScreen = false;

	XMenIni().Get("INIT", "skipLegalScreen", &skipLegalScreen);

	if (false)
	{
		ctx.eip = 0x4174BC;
	}
}

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

#include <UI/IMenuSystem.h>
#include <Misc/IClock.h>
#include <Misc/ICommand.h>
#include <Client/CClient.h>
#include <Scene/IScene.h>
#include <Scene/CScene.h>
#include <Game/CMarvelCamera.h>

static SafetyHookInline ogHook;

static void __fastcall UpdateFocusFreeViewHook(CMarvelCamera* camera, float camera_dx, float camera_dy, bool a, bool b, float moveXInput, float moveYInput)
{
	//printf("%f %f %f %f\n", camera_dx, camera_dy, moveXInput, moveYInput);
	ogHook.unsafe_thiscall(camera, camera_dx, camera_dy, a, b, moveXInput, moveYInput);
	//camera->mCurrentViewpoint = igVec3f::ZeroVector;
	//camera->mCurrentFocus = igVec3f::ZeroVector;
}

static void __fastcall UpdateGameBeginHook(CClient* client)
{
	client->UpdateGameBegin();
}

//void InputHook(SafetyHookContext& ctx)
//{
//	using namespace Memory::VP;
//	
//	//GUID_SysMouse = {6F1D2B60-D5A0-11CF-BFC7-444553540000}
//	LPDIRECTINPUT8 input = (LPDIRECTINPUT8)ctx.esi;
//	
//	if (input)
//	{
//		LPDIRECTINPUTDEVICE8 inputDevice = NULL;
//		input->CreateDevice(GUID_SysMouse, &inputDevice, NULL);
//
//		if (inputDevice)
//		{
//			uintptr_t pDirectInputDeviceVTable = *reinterpret_cast<uintptr_t*>(inputDevice);
//			uintptr_t pGetDeviceState = pDirectInputDeviceVTable + sizeof(uintptr_t) * 9;
//			uintptr_t pGetDeviceData = pDirectInputDeviceVTable + sizeof(uintptr_t) * 10;
//			
//			InterceptMemDisplacement(pGetDeviceState, orgGetDeviceState, GetDeviceStateHook);
//			InterceptMemDisplacement(pGetDeviceData, orgGetDeviceData, GetDeviceDataHook);
//
//			input->Release();
//		}
//	}
//}

#include <FX/CPlayFx.h>

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
	//static SafetyHookMid drawCollisionHook = safetyhook::create_mid(0x724156, DrawCollisionHook);
	//static SafetyHookMid menuFactoryCreateHook = safetyhook::create_mid(0x6BB85E, MenuFactoryCreateHook);
	static SafetyHookMid freeViewHook = safetyhook::create_mid(0x4B4CEC, FreeViewHook);
	static SafetyHookMid debugMoveHook = safetyhook::create_mid(0x46B19B, ActorMoveHook);
	
	//static SafetyHookMid skipLegalScreenHook = safetyhook::create_mid(0x417477, SkipLegalScreenHook);
	static SafetyHookInline updateGameBeginHook = safetyhook::create_inline(0x418690, UpdateGameBeginHook);

	//static SafetyHookMid beamFx1Hook = safetyhook::create_mid(0x48CB39, [](SafetyHookContext& ctx) {
	//	CPlayFx* playFx = (CPlayFx*)ctx.eax;
	//	printf("");
	//	});
	//
	//static SafetyHookMid beamFx2Hook = safetyhook::create_mid(0x48CDB0, [](SafetyHookContext& ctx) {
	//	CPlayFx* playFx = (CPlayFx*)ctx.eax;
	//	printf("");
	//	});
	//
	//static SafetyHookMid beamFx3Hook = safetyhook::create_mid(0x48D3EC, [](SafetyHookContext& ctx) {
	//	CPlayFx* playFx = (CPlayFx*)ctx.eax;
	//	printf("");
	//	});
	//
	//static SafetyHookMid beamFx4Hook = safetyhook::create_mid(0x48D4AA, [](SafetyHookContext& ctx) {
	//	CPlayFx* playFx = (CPlayFx*)ctx.eax;
	//	printf("");
	//	});

	//static SafetyHookMid inputHook = safetyhook::create_mid(0x41468A, InputHook);

	//ogHook = safetyhook::create_inline(0x4DFB90, UpdateFocusFreeViewHook);

	InterceptCall(GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3B3), orgReleaseVolatileResources, ReleaseVolatileResourcesHook);
	InterceptCall(GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3D5), orgRestoreVolatileResources, RestoreVolatileResourcesHook);
	
	InterceptMemDisplacement(0x720C8C + 1, orgsub_71EB20, sub_71EB20Hook);
	InterceptMemDisplacement(0x720CBD + 1, orgsub_71EB70, sub_71EB70Hook);

	//Patch(0x721A16 + 1, 1024);

	WriteMemDisplacement(0x4B0CAB + 1, VersionDataHandler);

	InjectHook(0x4AD5DA, TheDeveloperInput);
	InjectHook(0x4B0BEE, TheDeveloperInput);
	InjectHook(0x4B5503, TheDeveloperInput);

	//InjectHook(0x4E1F7F, UpdateFocusFreeViewHook);

	//InjectHook(0x417A4C, UpdateGameBegin);

	WriteMemDisplacement(0x7C02DC, ConfigSystemInFinal);

	////TODO: Shadow Tests
	//InjectHook(0x721A11, ShadowMapResolutionHook1, HookType::Jump);
	//Nop(0x721A1B, 1);
	//InjectHook(0x721A94, ShadowMapResolutionHook2, HookType::Jump);
	//Nop(0x721A99, 1);
	//
	//InjectHook(0x721A36, ShadowMapResolutionHook3, HookType::Jump);
	//Nop(0x721A36 + 5, 11);
	//InjectHook(0x721AB4, ShadowMapResolutionHook4, HookType::Jump);
	//Nop(0x721AB4 + 5, 11);
	//
	//Patch<unsigned char>(0x67B40D + 2, log2(shadowMapHemiSize));
	//Patch(0x67B554 + 3, shadowMapHemiSize);
	//Patch(0x664B80 + 1, shadowMapHemiSize);
	//Patch(0x664B85 + 1, shadowMapHemiSize);

	//texture0.texelsize
	//Patch(0x67AABD + 1, 1.0F / 2048.0F);//Xbox360 2048
	//Patch(0x67AAB8 + 1, 1.0F / 4096.0F);//Xbox360 4096
	//Nop(0x67B8F3, 5);

	//Nop(0x7204B7, 12);
	//
	//static SafetyHookMid displayTestHook = safetyhook::create_mid(0x7204B7, DisplayTestHook);

	OnClientPostInitEvent() += []()
	{
		TheDebugMenuMgr().Initialize();
	};

	OnClientShutdownEvent() += []()
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
		Command().RegisterCommand("give", GiveFunc);
		Command().RegisterCommand("slowmo", SlowMotionFunc);
		Command().RegisterCommand("reloadzonenopersistence", ReloadZoneNoPersistenceFunc);
	};

	OnHandleEventsEvent() += []()
	{
		TheDebugMenuMgr().RunFrame();
	};

	RegisterMenu<CMenuCodex>("CODEX_MENU");
	RegisterMenu<CMenuDebug>("DEBUG_MENU");
	RegisterMenu<CMenuFxEditor>("FXEDITOR_MENU");

	//InjectHook(0x790B92, WinMainHook);
}