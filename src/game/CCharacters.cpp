#include "CCharacters.h"

CCharacters* TheCharacters()
{
    return reinterpret_cast<CCharacters* (__cdecl*)()>(0x484700)();
}