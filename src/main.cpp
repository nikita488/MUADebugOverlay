#include <MUASDK.h>

#include "Plugins.h"

void OnInitializeHook()
{
	using namespace MUASDK;

#if _DEBUG
	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
#endif

	InstallHooks();
}