#include "CDebugMenuMgr.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>
#include "roboto_medium.h"

#include <igGapDisplay.h>
#include <igDisplay/igWin32Window.h>

#include <Display/IAlchemyDisplay.h>

#include "COverlayWindow.h"
#include "CLimitStatisticsWindow.h"
#include "CCacheWindow.h"
#include "CCameraWindow.h"
#include "CCheatsWindow.h"
#include "CConversationWindow.h"
#include "CEarthwormWindow.h"
#include "CFxWindow.h"
#include "CMiscWindow.h"
#include "CWorldWindow.h"
#include "CMapsWindow.h"

CDebugMenuMgr::CDebugMenuMgr()
{
#if _DEBUG
	mEnabled = true;
#else
	mEnabled = false;
#endif
}

CDebugMenuMgr::~CDebugMenuMgr()
{
}

void CDebugMenuMgr::Initialize()
{
	using namespace Display;
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.MouseDrawCursor = mEnabled;

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

void CDebugMenuMgr::Draw()
{
	if (!mEnabled || !ImGui::GetCurrentContext())
	{
		return;
	}
	
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImVec2& displaySize = ImGui::GetIO().DisplaySize;

	displaySize.x = float(TheDisplay().GetWidth());
	displaySize.y = float(TheDisplay().GetHeight());
	
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoDockingOverCentralNode | ImGuiDockNodeFlags_PassthruCentralNode);

	for (TDebugWindowVectorIterator it = mWindows.begin(); it != mWindows.end(); it++)
	{
		(*it)->Draw();
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
	if (mEnabled && ImGui::GetCurrentContext())
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
	if (mEnabled && ImGui::GetCurrentContext())
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
