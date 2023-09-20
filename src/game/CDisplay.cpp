#include "CDisplay.h"

HWND &CDisplay::hWnd = *reinterpret_cast<HWND*>(0xCD2678);
int &CDisplay::width = *reinterpret_cast<int*>(0xCD26DC);
int &CDisplay::height = *reinterpret_cast<int*>(0xCD26E0);
IDirect3D9 *&CDisplay::d3d = *reinterpret_cast<IDirect3D9**>(0xCD2734);
IDirect3DDevice9 *&CDisplay::device = *reinterpret_cast<IDirect3DDevice9**>(0xCD2730);

HWND CDisplay::getNativeWindowHandle()
{
    return reinterpret_cast<HWND (__thiscall*)()>(0x682700)();
}

IDirect3DDevice9* CDisplay::sub_71EFF0()
{
    return reinterpret_cast<IDirect3DDevice9* (__thiscall*)(CDisplay*)>(field_0[36])(this);
}

CDisplay* TheDisplay()
{
    return reinterpret_cast<CDisplay* (__cdecl*)()>(0x71FC60)();
}