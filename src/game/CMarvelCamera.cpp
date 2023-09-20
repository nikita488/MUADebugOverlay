#include "CMarvelCamera.h"

CMarvelCamera& _TheCamera = *reinterpret_cast<CMarvelCamera*>(0x8C76A8);

bool CMarvelCamera::sub_4DF1E0()
{
    return ((bool (__thiscall*)(CMarvelCamera*))field_0[42])(this);
}

int CMarvelCamera::sub_4D8F00()
{
    return ((int (__thiscall*)(CMarvelCamera*))field_0[43])(this);
}

bool CMarvelCamera::sub_4DF130()
{
    return ((bool (__thiscall*)(CMarvelCamera*))field_0[44])(this);
}

bool CMarvelCamera::sub_4DF140(bool a2)
{
    return ((bool (__thiscall*)(CMarvelCamera*, bool))field_0[45])(this, a2);
}

bool CMarvelCamera::sub_4DF170()
{
    return ((bool (__thiscall*)(CMarvelCamera*))field_0[46])(this);
}

bool CMarvelCamera::sub_4DF180(bool a2)
{
    return ((bool (__thiscall*)(CMarvelCamera*, bool))field_0[47])(this, a2);
}

int CMarvelCamera::sub_4DF240()
{
    return ((int (__thiscall*)(CMarvelCamera*))field_0[80])(this);
}

CMarvelCamera* TheCamera()
{
    return reinterpret_cast<CMarvelCamera* (__cdecl*)()>(0x4DF360)();
}
