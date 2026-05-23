#include "DebugOverlay.h"

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <MUASDK.h>
#include <Utils/MemoryMgr.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>

#include "IDebugMenuMgr.h"

#include "FxEditor/IFxEditor.h"
#include "FXEditor/CMenuFxEditor.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

static LRESULT(*orgsub_71EB20)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing);

static LRESULT sub_71EB20Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	HandleImguiInputs(hwnd, msg, wParam, lParam, keepProcessing);
	return orgsub_71EB20(hwnd, msg, wParam, lParam, keepProcessing);
}

static LRESULT(*orgsub_71EB70)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing);

static LRESULT sub_71EB70Hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	HandleImguiInputs(hwnd, msg, wParam, lParam, keepProcessing);
	return orgsub_71EB70(hwnd, msg, wParam, lParam, keepProcessing);
}

static void(__thiscall* orgReleaseVolatileResources)(void* vc);

static void __fastcall ReleaseVolatileResourcesHook(void* vc)
{
	orgReleaseVolatileResources(vc);
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

static void(__thiscall* orgRestoreVolatileResources)(void* vc);

static void __fastcall RestoreVolatileResourcesHook(void* vc)
{
	orgRestoreVolatileResources(vc);
	ImGui_ImplDX9_CreateDeviceObjects();
}

static HRESULT(WINAPI* orgGetDeviceState)(LPDIRECTINPUTDEVICE8 device, DWORD cbData, LPVOID lpvData);

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

static HRESULT(WINAPI* orgGetDeviceData)(LPDIRECTINPUTDEVICE8 device, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);

static HRESULT WINAPI GetDeviceDataHook(LPDIRECTINPUTDEVICE8 device, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	if (TheFxEditor().IsActive() || TheDebugMenuMgr().WantCaptureMouse())
	{
		*pdwInOut = 0;
		return DI_OK;
	}

	return orgGetDeviceData(device, cbObjectData, rgdod, pdwInOut, dwFlags);
}

DebugOverlay::DebugOverlay()
{
	using namespace Memory::VP;
	using namespace MUASDK;

	LPDIRECTINPUT8 input = NULL;

	if (DirectInput8Create((HMODULE)&__ImageBase, DIRECTINPUT_VERSION, IID_IDirectInput8W, (LPVOID*)&input, NULL) == DI_OK)
	{
		LPDIRECTINPUTDEVICE8 inputDevice = NULL;

		if (input->CreateDevice(GUID_SysMouse, &inputDevice, NULL) == DI_OK)
		{
			intptr_t pDirectInputDeviceVTable = *reinterpret_cast<intptr_t*>(inputDevice);
			intptr_t pGetDeviceState = pDirectInputDeviceVTable + sizeof(intptr_t) * 9;
			intptr_t pGetDeviceData = pDirectInputDeviceVTable + sizeof(intptr_t) * 10;

			InterceptMemDisplacement(pGetDeviceState, orgGetDeviceState, GetDeviceStateHook);
			InterceptMemDisplacement(pGetDeviceData, orgGetDeviceData, GetDeviceDataHook);

			inputDevice->Release();
		}

		input->Release();
	}

	InterceptCall(GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3B3), orgReleaseVolatileResources, ReleaseVolatileResourcesHook);
	InterceptCall(GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3D5), orgRestoreVolatileResources, RestoreVolatileResourcesHook);

	InterceptMemDisplacement(0x720C8C + 1, orgsub_71EB20, sub_71EB20Hook);
	InterceptMemDisplacement(0x720CBD + 1, orgsub_71EB70, sub_71EB70Hook);

	OnClientPreInitEvent() += []()
	{
		RegisterMenu<CMenuFxEditor>("FXEDITOR_MENU");//TODO: Split to separate ASI plugin
	};

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
	
	OnHandleEventsEvent() += []()
	{
		TheDebugMenuMgr().RunFrame();
	};
}

DebugOverlay plugin;
