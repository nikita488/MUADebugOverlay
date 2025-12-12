#include "CMapsWindow.h"

#if !defined(RATL_STRING_VS_INC)
	#include <Common/Ratl/string_vs.h>
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>

#include <Misc/tinyXmlBinary.h>
#include <Misc/Common.h>
#include <Misc/ICommand.h>
#include <Game/IGame.h>
#include <Game/IZoneInfo.h>

CMapsWindow::CMapsWindow() : mSelectedLoadpoint(-1), mSelectedZone(-1)
{
}

void CMapsWindow::LoadPointList()
{
	using namespace TinyXmlBinary;

	TiXmlbDocument doc;

	if (doc.LoadFile("data/loadpoint_list.xmlb", true, MP_TEMPORARY, false))
	{
		TiXmlElement* root = doc.FirstChild("loadpointlist");
		TiXmlElement* child = NULL;

		while (child = root->IterateChildren(child))
		{
			if (child->Value() == "loadpoint")
			{
				const TiXmlString* value = child->Attribute("value");
				const TiXmlString* build = child->Attribute("build");

				if (value)
				{
					const char* buildStr = build ? build->c_str() : "";

					if (TheZoneInfoSystem().TestBuild(buildStr))
					{
						mLoadPoints.push_back(value->c_str());
					}
				}
			}
		}
	}
}

void CMapsWindow::Initialize()
{
	LoadPointList();
}

void CMapsWindow::Shutdown()
{
	mLoadPoints.clear();
}

void CMapsWindow::LoadSelectedLoadPoint()
{
	if (mSelectedLoadpoint < 0)
	{
		return;
	}
	
	auto& loadPoint = mLoadPoints[mSelectedLoadpoint];

	ratl::string_vs<256> name;
	FormatToStringBuffer(name.c_str(), 256, "loadpoint %s", loadPoint.c_str());

	Command().AsyncCommand(name.c_str());
}

void CMapsWindow::DrawLoadPointList()
{
	if (ImGui::BeginTabItem("Load Points"))
	{		
		if (ImGui::BeginListBox("##LoadPointList", ImVec2(-1.0F, -ImGui::GetFrameHeightWithSpacing())))
		{
			for (auto it = mLoadPoints.begin(); it != mLoadPoints.end(); it++)
			{
				size_t index = it - mLoadPoints.begin();
				bool selected = index == mSelectedLoadpoint;

				if (ImGui::Selectable(it->c_str(), selected))
				{
					mSelectedLoadpoint = index;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}

		if (ImGui::Button("Load", ImVec2(-1.0F, 0.0F)))
		{
			LoadSelectedLoadPoint();
		}

		ImGui::EndTabItem();
	}
}

void CMapsWindow::LoadSelectedZone()
{
	if (mSelectedZone < 0)
	{
		return;
	}

	Command().ExecuteCommandString("resetgame");

	IZoneInfo& zoneInfo = TheZoneInfoSystem().GetZoneInfo(mSelectedZone + 1);
	TheGame().SetCurrentAct(zoneInfo.GetAct(), true);

	ratl::string_vs<256> command;
	FormatToStringBuffer(command.c_str(), 256, "loadmapaddteam %s", zoneInfo.GetZoneName().c_str());

	Command().AsyncCommand(command.c_str());
}

void CMapsWindow::DrawZoneList()
{
	if (ImGui::BeginTabItem("Zones"))
	{				
		if (ImGui::BeginListBox("##ZoneList", ImVec2(-1.0F, -ImGui::GetFrameHeightWithSpacing())))
		{
			for (int i = 0; i < TheZoneInfoSystem().GetNumZones(); i++)
			{
				IZoneInfo& zoneInfo = TheZoneInfoSystem().GetZoneInfo(i + 1);
				bool selected = i == mSelectedZone;

				if (ImGui::Selectable(zoneInfo.GetZoneName().c_str(), selected))
				{
					mSelectedZone = i;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}

		if (ImGui::Button("Load", ImVec2(-1.0F, 0.0F)))
		{			
			LoadSelectedZone();
		}

		ImGui::EndTabItem();
	}
}

void CMapsWindow::Draw()
{
	if (ImGui::Begin("Maps"))
	{
		if (ImGui::BeginTabBar("##MapsTabBar"))
		{
			DrawZoneList();
			DrawLoadPointList();
			
			ImGui::EndTabBar();
		}
	}

	ImGui::End();
}