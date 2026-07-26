#include "CDebugMenuMgr.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>
#include "Utils/roboto_medium.h"

#include <igGapDisplay.h>
#include <igDisplay/igWin32Window.h>

#include <Display/IAlchemyDisplay.h>

#include "DebugWindows/COverlayWindow.h"
#include "DebugWindows/CLimitStatisticsWindow.h"
#include "DebugWindows/CCacheWindow.h"
#include "DebugWindows/CCameraWindow.h"
#include "DebugWindows/CCheatsWindow.h"
#include "DebugWindows/CConversationWindow.h"
#include "DebugWindows/CEarthwormWindow.h"
#include "DebugWindows/CFxWindow.h"
#include "DebugWindows/CMiscWindow.h"
#include "DebugWindows/CWorldWindow.h"
#include "DebugWindows/CMapsWindow.h"
#include "DebugWindows/CCharStatsWindow.h"

#ifdef _DEBUG
#include "FXEditor/CFxEditorWindow.h"
#endif

#include "FXEditor/IFxEditor.h"

void CDebugMenuMgr::Initialize()
{
	using namespace Display;
	
#if _DEBUG
	mEnabled = true;
#else
	mEnabled = false;
#endif
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromMemoryCompressedTTF(roboto_medium_compressed_data, roboto_medium_compressed_size, 24.0F);

	ImGui::StyleColorsDark();

	igWin32Window* wnd = igWin32Window::dynamicCast(TheAlchemyDisplay().GetWindow());
	HWND hWnd = wnd ? wnd->getNativeWindowHandle() : NULL;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(TheDisplay().GetDevice());

	RegisterAllWindows();

	for (TDebugWindowVectorIterator it = mWindows.begin(); it != mWindows.end(); it++)
	{
		(*it)->Initialize();
	}

	//mFxEditor.Initialize();
}

void CDebugMenuMgr::RegisterAllWindows()
{
	COverlayWindow* overlay = RegisterWindow<COverlayWindow>("overlay");
	CMiscWindow* misc = RegisterWindow<CMiscWindow>("misc");

	RegisterWindow<CLimitStatisticsWindow>("limit_stats");
	RegisterWindow<CCacheWindow>("cache");
	RegisterWindow<CCameraWindow>("camera");
	RegisterWindow<CCheatsWindow>("cheats");
	RegisterWindow<CConversationWindow>("conversation");
	RegisterWindow<CEarthwormWindow>("earthworm");
	RegisterWindow<CFxWindow>("fx");
	RegisterWindow<CWorldWindow>("world");
	RegisterWindow<CMapsWindow>("maps");
	RegisterWindow<CCharStatsWindow>("character_stats");
	
#ifdef _DEBUG
	RegisterWindow<CFxEditorWindow>("fx_editor");
#endif

	misc->SetOverlay(overlay);
}

void CDebugMenuMgr::Shutdown()
{
	for (TDebugWindowVectorIterator it = mWindows.begin(); it != mWindows.end(); it++)
	{
		(*it)->Shutdown();
	}
	
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
	mWindowsLookup.clear();
	
	for (TDebugWindowVectorIterator it = mWindows.begin(); it != mWindows.end(); it++)
	{
		delete *it;
	}

	mWindows.clear();
}

void CDebugMenuMgr::RunFrame()
{
	//if (!ImGui::GetCurrentContext()) return;
	//
	//ImGui_ImplDX9_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//
	//ImGuiIO& io = ImGui::GetIO();
	//
	//io.MouseDrawCursor = mEnabled || TheFxEditor().IsActive();
	//io.DisplaySize.x = float(TheDisplay().GetWidth());
	//io.DisplaySize.y = float(TheDisplay().GetHeight());
	//
	//ImGui::NewFrame();
	//ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoDockingOverCentralNode | ImGuiDockNodeFlags_PassthruCentralNode);
}

void CDebugMenuMgr::Draw()
{
	if (!ImGui::GetCurrentContext()) return;
	
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	
	ImGuiIO& io = ImGui::GetIO();
	
	io.MouseDrawCursor = mEnabled || TheFxEditor().IsActive();
	io.DisplaySize.x = float(TheDisplay().GetWidth());
	io.DisplaySize.y = float(TheDisplay().GetHeight());
	
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoDockingOverCentralNode | ImGuiDockNodeFlags_PassthruCentralNode);

	TheFxEditor().RunFrame();
	
	if (mEnabled)
	{
		for (TDebugWindowVectorIterator it = mWindows.begin(); it != mWindows.end(); it++)
		{
			(*it)->Draw();
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

bool CDebugMenuMgr::GetEnabled() const
{
	return mEnabled;
}

void CDebugMenuMgr::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}

bool CDebugMenuMgr::WantCaptureMouse() const
{	
	if ((mEnabled || TheFxEditor().IsActive()) && ImGui::GetCurrentContext())
	{
		return ImGui::GetIO().WantCaptureMouse;
	}
	else
	{
		return false;
	}
}

bool CDebugMenuMgr::WantCaptureKeyboard() const
{
	if ((mEnabled || TheFxEditor().IsActive()) && ImGui::GetCurrentContext())
	{
		return ImGui::GetIO().WantCaptureKeyboard;
	}
	else
	{
		return false;
	}
}

IDebugMenuMgr& TheDebugMenuMgr()
{
	static CDebugMenuMgr mgr;
	return mgr;
}
