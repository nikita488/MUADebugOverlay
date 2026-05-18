#include "CCacheWindow.h"

#include <imgui/imgui.h>

#include <Misc/CCacheHandlePool.h>

void CCacheWindow::Draw()
{
	if (ImGui::Begin("Cached Resources"))
	{
		CCacheHandlePool& cacheHandlePool = TheCacheHandlePool();
		
		ImGui::Text("%d / %d", cacheHandlePool.mCacheHandleLookup.size(), cacheHandlePool.mCacheHandleLookup.capacity());

		int index = 0;

		for (auto it = cacheHandlePool.mCacheHandleLookup.begin(); it != cacheHandlePool.mCacheHandleLookup.end(); it++)
		{
			SCacheHandle& handle = it.value();

			ImGui::Text("%02d: %s", index + 1, it.key().c_str());
			index++;
		}
	}

	ImGui::End();
}
