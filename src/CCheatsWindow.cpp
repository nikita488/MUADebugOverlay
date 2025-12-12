#include "CCheatsWindow.h"

#include <imgui/imgui.h>

#include <Game/ICheats.h>

void CCheatsWindow::Draw()
{
	if (ImGui::Begin("Cheats"))
	{
		ICheats& cheats = TheCheats();

		bool c0 = cheats.Get(ICheats::CHEAT_00, false);
		bool c1 = cheats.Get(ICheats::CHEAT_01, false);
		bool c2 = cheats.Get(ICheats::CHEAT_02, false);
		bool c3 = cheats.Get(ICheats::CHEAT_03, false);
		bool c4 = cheats.Get(ICheats::CHEAT_04, false);
		bool c5 = cheats.Get(ICheats::CHEAT_05, false);
		bool c6 = cheats.Get(ICheats::CHEAT_06, false);
		bool c7 = cheats.Get(ICheats::CHEAT_07, false);
		bool c8 = cheats.Get(ICheats::CHEAT_08, false);
		bool c9 = cheats.Get(ICheats::CHEAT_09, false);
		bool c10 = cheats.Get(ICheats::CHEAT_10, false);
		bool c11 = cheats.Get(ICheats::CHEAT_11, false);
		bool c12 = cheats.Get(ICheats::CHEAT_12, false);
		bool c13 = cheats.Get(ICheats::CHEAT_13, false);
		bool c14 = cheats.Get(ICheats::CHEAT_14, false);
		bool c15 = cheats.Get(ICheats::CHEAT_15, false);

		if (ImGui::Checkbox("All Costumes", &c0))
		{
			cheats.SetEnabled(ICheats::CHEAT_00, c0);
			cheats.Set(ICheats::CHEAT_00, c0);
		}

		if (ImGui::Checkbox("God Mode", &c1))
		{
			cheats.SetEnabled(ICheats::CHEAT_01, c1);
			cheats.Set(ICheats::CHEAT_01, c1);
		}

		if (ImGui::Checkbox("100,000 Coins", &c2))
		{
			cheats.SetEnabled(ICheats::CHEAT_02, c2);
			cheats.Set(ICheats::CHEAT_02, c2);
		}

		if (ImGui::Checkbox("Touch of Death", &c3))
		{
			cheats.SetEnabled(ICheats::CHEAT_03, c3);
			cheats.Set(ICheats::CHEAT_03, c3);
		}
		if (ImGui::Checkbox("Super Speed", &c4))
		{
			cheats.SetEnabled(ICheats::CHEAT_04, c4);
			cheats.Set(ICheats::CHEAT_04, c4);
		}

		if (ImGui::Checkbox("All Powers", &c5))
		{
			cheats.SetEnabled(ICheats::CHEAT_05, c5);
			cheats.Set(ICheats::CHEAT_05, c5);
		}
		if (ImGui::Checkbox("Level 99", &c6))
		{
			cheats.SetEnabled(ICheats::CHEAT_06, c6);
			cheats.Set(ICheats::CHEAT_06, c6);
		}

		if (ImGui::Checkbox("Xtreme Filler", &c7))
		{
			cheats.SetEnabled(ICheats::CHEAT_07, c7);
			cheats.Set(ICheats::CHEAT_07, c7);
		}

		if (ImGui::Checkbox("All Characters", &c8))
		{
			cheats.SetEnabled(ICheats::CHEAT_08, c8);
			cheats.Set(ICheats::CHEAT_08, c8);
		}

		if (ImGui::Checkbox("All Comics", &c9))
		{
			cheats.SetEnabled(ICheats::CHEAT_09, c9);
			cheats.Set(ICheats::CHEAT_09, c9);
		}
		if (ImGui::Checkbox("All Concept Art", &c10))
		{
			cheats.SetEnabled(ICheats::CHEAT_10, c10);
			cheats.Set(ICheats::CHEAT_10, c10);
		}

		if (ImGui::Checkbox("All Cinematics", &c11))
		{
			cheats.SetEnabled(ICheats::CHEAT_11, c11);
			cheats.Set(ICheats::CHEAT_11, c11);
		}

		if (ImGui::Checkbox("All Wallpapers", &c12))
		{
			cheats.SetEnabled(ICheats::CHEAT_12, c12);
			cheats.Set(ICheats::CHEAT_12, c12);
		}

		if (ImGui::Checkbox("All Courses", &c13))
		{
			cheats.SetEnabled(ICheats::CHEAT_13, c13);
			cheats.Set(ICheats::CHEAT_13, c13);
		}

		if (ImGui::Checkbox("Unlock Daredevil", &c14))
		{
			cheats.SetEnabled(ICheats::CHEAT_14, c14);
			cheats.Set(ICheats::CHEAT_14, c14);
		}

		if (ImGui::Checkbox("Unlock Silver Surfer", &c15))
		{
			cheats.SetEnabled(ICheats::CHEAT_15, c15);
			cheats.Set(ICheats::CHEAT_15, c15);
		}
	}

	ImGui::End();
}