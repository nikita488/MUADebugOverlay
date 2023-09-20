#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "game/CClient.h"
#include "game/CDisplay.h"
#include "game/CMarvelCamera.h"
#include "game/CCharacters.h"
#include "game/CCheats.h"
#include "hstring.h"
#include "patcher/patcher.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HINSTANCE hDLLModule;
#if _DEBUG
static bool overlayEnabled = true;
#else
static bool overlayEnabled = false;
#endif

static void DrawImguiMenus()
{
	if (!overlayEnabled)
		return;

	CCheats* cheats = TheCheats();
	CMarvelCamera* camera = TheCamera();
	CCharacters* characters = TheCharacters();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode);

	if (camera)
	{
		if (ImGui::Begin("Camera"))
		{
			bool freeViewEnabled = camera->sub_4DF1E0();

			if (ImGui::Checkbox("Free View", &freeViewEnabled))
				camera->sub_4D8F00();
		}

		ImGui::End();
	}

	if (cheats)
	{
		if (ImGui::Begin("Cheats"))
		{
			unsigned int& flags = cheats->field_8;

			ImGui::CheckboxFlags("All Costumes", &flags, 1 << 0);
			ImGui::CheckboxFlags("God Mode", &flags, 1 << 1);
			ImGui::CheckboxFlags("100,000 Coins", &flags, 1 << 2);
			ImGui::CheckboxFlags("Touch of Death", &flags, 1 << 3);
			ImGui::CheckboxFlags("Super Speed", &flags, 1 << 4);
			ImGui::CheckboxFlags("All Powers", &flags, 1 << 5);
			ImGui::CheckboxFlags("Level 99", &flags, 1 << 6);
			ImGui::CheckboxFlags("Xtreme Filler", &flags, 1 << 7);
			ImGui::CheckboxFlags("All Characters", &flags, 1 << 8);
			ImGui::CheckboxFlags("All Comics", &flags, 1 << 9);
			ImGui::CheckboxFlags("All Concept Art", &flags, 1 << 10);
			ImGui::CheckboxFlags("All Cinematics", &flags, 1 << 11);
			ImGui::CheckboxFlags("All Wallpapers", &flags, 1 << 12);
			ImGui::CheckboxFlags("All Courses", &flags, 1 << 13);
			ImGui::CheckboxFlags("Unlock Daredevil", &flags, 1 << 14);
			ImGui::CheckboxFlags("Unlock Silver Surfer", &flags, 1 << 15);
		}

		ImGui::End();
	}

	if (characters && characters->field_104248)
	{
		if (ImGui::Begin("Limit Statistics"))
		{
			void* CMultipartManager = reinterpret_cast<void*>(0x8C8718);
			void* CFightStyle = *reinterpret_cast<void**>(0x934B40);
			void* CCombatStyles = *reinterpret_cast<void**>(0x934BF4);
			void* CCombatNode = *reinterpret_cast<void**>(0x934B3C);
			void* CUnknown = *reinterpret_cast<void**>(0x934B34);

			int talentStartInfoCount = *reinterpret_cast<int*>(0x8090BC);
			int statEffectsCount = *reinterpret_cast<int*>(0x912EC4);
			int boltOnCount = *reinterpret_cast<int*>(0x911C9C);
			int multipartManagerCount = *reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(CMultipartManager) + 2904);
			int fightStyleCount = *reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(CCombatStyles) + 4 + 12 + 656);
			int fightMoveNodeCount = *reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(CFightStyle) + 27468);
			int combatNodeEventCount = *reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(CCombatStyles) + 4144 + 123564);
			int combatNodeTriggerCount = *reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(CCombatNode) + 18636);
			int combatNodeChainCount = *reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(CUnknown) + 32908);
			int requireCount = *reinterpret_cast<int*>(0x8F930C);

			ImGui::Text("CStats: %d / %d", characters->field_54632, MAX_STATS);
			ImGui::Text("SCharacterKey: %d / %d", characters->field_73072, 350);
			ImGui::Text("Hero: %d / %d", characters->field_106848, MAX_HEROS);
			ImGui::Text("NPC: %d / %d", characters->field_108796, 13);
			ImGui::Text("Talent Start Info: %d / %d", talentStartInfoCount, 70);
			ImGui::Text("Stat Effect: %d / %d", statEffectsCount, 36);
			ImGui::Text("BoltOn: %d / %d", boltOnCount, 32);
			ImGui::Text("Characters with Multipart's: %d / %d", multipartManagerCount, 20);
			ImGui::Text("FightStyle: %d / %d", fightStyleCount, 32);
			ImGui::Text("FightMove (Combat Node): %d / %d", fightMoveNodeCount, 380);//max combat nodes count (fightstyle nodes)?
			ImGui::Text("Combat Node Event: %d / %d", combatNodeEventCount, 1400); //combat node event pool max 1400
			ImGui::Text("Combat Node Trigger: %d / %d", combatNodeTriggerCount, 1520);//CCombatNode::AddTriggerToList() - too many combat node triggers! <max 1520>
			ImGui::Text("Combat Node Chain: %d / %d", combatNodeChainCount, 228);
			ImGui::Text("Requirements: %d / %d", requireCount, 300);
		}

		ImGui::End();

		if (ImGui::Begin("CStats"))
		{
			for (unsigned int i = 0; i < characters->field_54632; i++)
			{
				CStats stats = characters->field_12[i];

				char label[64];
				snprintf(label, 64, "%d: %s", i, stats.mName);

				if (ImGui::TreeNode(label))
				{
					ImGui::Text("Strength: %d", stats.mAttributes.mStrength);
					ImGui::Text("Body: %d", stats.mAttributes.mBody);
					ImGui::Text("Mind: %d", stats.mAttributes.mMind);
					ImGui::Text("Level: %d", stats.mAttributes.mLevel);
					ImGui::Text("AI Level: %d", stats.mAttributes.mAILevel);
					ImGui::Text("AI Power: %d", stats.mAttributes.mAIPower);
					ImGui::Text("Talents: %d / %d", stats.mAttributes.field_284, 32);
					ImGui::Text("Power 1: %s", stats.field_292);
					ImGui::Text("Power 2: %s", stats.field_314);
					ImGui::Text("Power 3: %s", stats.field_336);
					ImGui::Text("Power 4: %s", stats.field_358);
					ImGui::Text("Current Skin: %s", stats.mSkinNames[stats.field_380].c_str());

					unsigned int boltons = 0;

					for (unsigned int j = 0; j < 7; j++)
					{
						if (stats.mBoltOns[j] > 0)
							boltons++;
					}

					ImGui::Text("BoltOn's: %d / %d", boltons, 7);
					ImGui::Text("Stat Effect's: %d / %d", stats.mStatEffectsSize, 7);
					ImGui::Text("Name: %s", stats.mName);
					ImGui::Text("Character Name: %s", stats.mCharacterName);
					ImGui::Text("Powerstyle: %s", stats.mPowerstyle);
					ImGui::Text("Moveset: %s", stats.mMoveset);
					ImGui::Text("Character Anims: %s", stats.mCharacterAnims);
					ImGui::Text("Character Anims Class: %s", stats.mCharacterAnimClass);
					ImGui::Text("Footstep FX: %s", stats.mFootstepFX);
					ImGui::Text("Sound Dir: %s", stats.mSoundDir);

					if (ImGui::TreeNode("Skins"))
					{
						for (unsigned int j = 0; j < 6; j++)
							ImGui::Text("%d: %s", j + 1, stats.mSkinNames[j].c_str());
						ImGui::TreePop();
					}

					ImGui::Text("Character Index: %d", stats.field_780);
					ImGui::Text("Heaviness: %d", stats.mHeaviness);
					ImGui::Text("Menu Location: %d", stats.mMenuLocation);
					ImGui::Text("Size: %f %f %f", stats.mSize[0], stats.mSize[1], stats.mSize[2]);
					ImGui::Text("Team: %d", stats.mTeam);
					ImGui::Text("Has Weapon: %s", stats.mWeapon > 0 ? "true" : "false");
					ImGui::Text("Combo Level: %d", stats.mComboLevel);

					if (ImGui::TreeNode("Flags 1"))
					{
						unsigned int flags = unsigned int(stats.field_813);

						ImGui::CheckboxFlags("willflee", &flags, 1);
						ImGui::CheckboxFlags("canFly", &flags, 2);
						ImGui::CheckboxFlags("canSeeStealthed", &flags, 4);
						ImGui::CheckboxFlags("large", &flags, 8);
						ImGui::CheckboxFlags("teleportpathfail", &flags, 16);
						ImGui::CheckboxFlags("xpexempt", &flags, 32);
						ImGui::CheckboxFlags("ignoreboundsscaling", &flags, 64);
						ImGui::CheckboxFlags("canbeallythrown", &flags, 128);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Flags 2"))
					{
						unsigned int flags = unsigned int(stats.field_814);

						ImGui::CheckboxFlags("aiforceranged", &flags, 1);
						ImGui::CheckboxFlags("ainomelee", &flags, 2);
						ImGui::CheckboxFlags("ainostraffe", &flags, 4);
						ImGui::CheckboxFlags("ainocover", &flags, 8);
						ImGui::CheckboxFlags("unknown", &flags, 16);
						ImGui::CheckboxFlags("canthrowally", &flags, 32);
						ImGui::CheckboxFlags("selectable", &flags, 64);
						ImGui::CheckboxFlags("nonhumanoidskeleton", &flags, 128);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Flags 3"))
					{
						unsigned int flags = unsigned int(stats.field_815);

						ImGui::CheckboxFlags("standard_pain_only", &flags, 1);
						ImGui::CheckboxFlags("scaleattacks", &flags, 2);
						ImGui::CheckboxFlags("targetheight", &flags, 4);
						ImGui::CheckboxFlags("alwaysFlying", &flags, 8);
						ImGui::CheckboxFlags("isteam", &flags, 16);
						ImGui::CheckboxFlags("weapon_no_acc", &flags, 32);
						ImGui::CheckboxFlags("reserved1", &flags, 64);
						ImGui::CheckboxFlags("reserved2", &flags, 128);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Race"))
					{
						unsigned int flags = unsigned int(stats.mRace);

						ImGui::CheckboxFlags("Human", &flags, 2);
						ImGui::CheckboxFlags("Mutant", &flags, 4);
						ImGui::CheckboxFlags("Robot", &flags, 8);
						ImGui::CheckboxFlags("Sentinel", &flags, 16);
						ImGui::CheckboxFlags("Morlock", &flags, 32);
						ImGui::CheckboxFlags("Shadow", &flags, 64);
						ImGui::CheckboxFlags("Astral", &flags, 128);
						ImGui::CheckboxFlags("XMen", &flags, 256);
						ImGui::CheckboxFlags("Brotherhood", &flags, 512);
						ImGui::TreePop();
					}

					ImGui::Text("Scale Factor: %f", stats.mScaleFactor);
					ImGui::Text("Has Multipart's: %s", stats.mMultipartManager > 0 ? "true" : "false");
					ImGui::Text("Counter: %d", stats.mCounter);
					ImGui::Text("Alert Radius: %d", stats.mAlertRadius);
					ImGui::Text("Attack Range: %d", stats.mAttackRange);
					ImGui::Text("Grab Chance: %f", stats.mGrabChance);
					ImGui::Text("Pickup Throw Chance: %f", stats.mPickupThrowChance);
					ImGui::Text("Script Level: %d", stats.mScriptLevel);
					ImGui::Text("Flee Distance: %f", stats.mFleeDistance);
					ImGui::Text("Melee Timer Offset: %f", stats.mMeleeTimerOffset);
					ImGui::Text("Melee Timer Random Add: %f", stats.mMeleeTimerRandomAdd);
					ImGui::Text("Ranged Timer Offset: %f", stats.mRangedTimerOffset);
					ImGui::Text("Danger Rating: %f", stats.mDangerRating);
					ImGui::Text("AI Pickup Weapon Chance: %f", stats.mAIPickupWeaponChance);

					ImGui::TreePop();
				}
			}
		}

		ImGui::End();
	}
}

static void HandleImguiInputs(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	if (!ImGui::GetCurrentContext())
		return;

	if (msg == WM_KEYDOWN && wParam == VK_F6)
		ImGui::GetIO().MouseDrawCursor = overlayEnabled = !overlayEnabled;

	if (overlayEnabled)
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
}

static LRESULT (*sub_71EB20)(HWND, UINT, WPARAM, LPARAM, bool*);//CDisplay window callback 1

static LRESULT sub_71EB20_hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	HandleImguiInputs(hwnd, msg, wParam, lParam, keepProcessing);
	return sub_71EB20(hwnd, msg, wParam, lParam, keepProcessing);
}

static LRESULT (*sub_71EB70)(HWND, UINT, WPARAM, LPARAM, bool*);//CDisplay window callback 2

static LRESULT sub_71EB70_hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, bool* keepProcessing)
{
	HandleImguiInputs(hwnd, msg, wParam, lParam, keepProcessing);
	return sub_71EB70(hwnd, msg, wParam, lParam, keepProcessing);
}

static bool (__thiscall *sub_71F660)(CDisplay*);//CDisplay end draw

static bool __fastcall OnDisplayEndDraw(CDisplay* display)
{
	if (overlayEnabled && ImGui::GetCurrentContext())
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::GetIO().DisplaySize = ImVec2(float(CDisplay::width), float(CDisplay::height));
		ImGui::NewFrame();
		DrawImguiMenus();
		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
	return sub_71F660(display);
}

static void (__thiscall* sub_417290)(CClient*);//CClient initialize

static void __fastcall OnClientInitialize(CClient* client)
{
	sub_417290(client);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.MouseDrawCursor = overlayEnabled;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(CDisplay::hWnd);
	ImGui_ImplDX9_Init(CDisplay::device);
}

static void (__thiscall* sub_417730)(CClient*);//CClient shutdown

static void __fastcall OnClientShutdown(CClient* client)
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	sub_417730(client);
}

static HRESULT (__stdcall* _GetDeviceState)(LPDIRECTINPUTDEVICE8, DWORD, LPVOID);

static HRESULT __stdcall OnGetDeviceState(LPDIRECTINPUTDEVICE8 device, DWORD cbData, LPVOID lpvData)
{
	if (overlayEnabled && ImGui::GetCurrentContext())
	{
		ImGuiIO& io = ImGui::GetIO();

		if ((cbData == 256 && io.WantCaptureKeyboard) || (cbData == sizeof(DIMOUSESTATE2) && io.WantCaptureMouse))
		{
			ZeroMemory(lpvData, cbData);
			return DI_OK;
		}
	}

	return _GetDeviceState(device, cbData, lpvData);
}

static HRESULT (__stdcall* _GetDeviceData)(LPDIRECTINPUTDEVICE8, DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);

static HRESULT __stdcall OnGetDeviceData(LPDIRECTINPUTDEVICE8 device, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	if (overlayEnabled && ImGui::GetCurrentContext())
	{
		ImGuiIO& io = ImGui::GetIO();

		if (io.WantCaptureMouse)
		{
			*pdwInOut = 0;
			return DI_OK;
		}
	}

	return _GetDeviceData(device, cbObjectData, rgdod, pdwInOut, dwFlags);
}

static void (__thiscall *_releaseVolatileResources)(void*);

static void __fastcall OnReleaseVolatileResources(void* visualContext)
{
	_releaseVolatileResources(visualContext);
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

static void (__thiscall *_restoreVolatileResources)(void*);

static void __fastcall OnRestoreVolatileResources(void* visualContext)
{
	_restoreVolatileResources(visualContext);
	ImGui_ImplDX9_CreateDeviceObjects();
}

static void InstallHooks()
{
#if _DEBUG
	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
#endif

	HRESULT result;
	LPDIRECTINPUT8 input = NULL;

	if ((result = DirectInput8Create(hDLLModule, DIRECTINPUT_VERSION, IID_IDirectInput8W, (LPVOID*)&input, NULL)) != DI_OK)
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

	InterceptVmethod(&_GetDeviceState, OnGetDeviceState, pGetDeviceState);
	InterceptVmethod(&_GetDeviceData, OnGetDeviceData, pGetDeviceData);
	inputDevice->Release();
	input->Release();

	InterceptCall(&_releaseVolatileResources, OnReleaseVolatileResources, GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3B3));
	InterceptCall(&_restoreVolatileResources, OnRestoreVolatileResources, GetExternalAddress("libIGGfx.dll", 0x1A00000, 0x1A5E3D5));

	InterceptCallback(&sub_71EB20, sub_71EB20_hook, 0x720C8C);
	InterceptCallback(&sub_71EB70, sub_71EB70_hook, 0x720CBD);
	
	InterceptVmethod(&sub_417290, OnClientInitialize, 0x7996C8);
	InterceptVmethod(&sub_417730, OnClientShutdown, 0x7996CC);
	InterceptVmethod(&sub_71F660, OnDisplayEndDraw, 0x7CD80C);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		hDLLModule = hinstDLL;
	}
	return TRUE;
}

extern "C"
{
	static LONG InitCount = 0;
	__declspec(dllexport) void InitializeASI()
	{
		if (_InterlockedCompareExchange(&InitCount, 1, 0) != 0) return;
		InstallHooks();
	}
}