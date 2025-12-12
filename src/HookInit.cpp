#include <windows.h>

#define HOOKED_FUNCTION GetStartupInfoA
#define HOOKED_LIBRARY "KERNEL32.DLL"

#include <Utils/HookInit.hpp>