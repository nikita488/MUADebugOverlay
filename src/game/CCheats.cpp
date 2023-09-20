#include "CCheats.h"

CCheats* TheCheats()
{
    return reinterpret_cast<CCheats* (__cdecl*)()>(0x486530)();
}