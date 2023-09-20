#include "hstring.h"

TStrPool& Pool()
{
    return reinterpret_cast<TStrPool& (__cdecl*)()>(0x72CB60)();
}

hstring::hstring()
{
    mHandle = 0;
}

hstring::hstring(const char* str)
{
    init(str);
}

hstring::hstring(const hstring& str)
{
    mHandle = str.mHandle;
}

void hstring::init(const char* str)
{
    reinterpret_cast<void (__thiscall*)(hstring*, const char*)>(0x448980)(this, str);
}

hstring& hstring::operator=(const char* str)
{
    init(str);
    return *this;
}

hstring& hstring::operator=(const hstring& str)
{
    mHandle = str.mHandle;
    return *this;
}

const char* hstring::c_str(void) const
{
    if (!mHandle)
        return "";
    return ((const char*)Pool()[mHandle]);
}

const char* hstring::operator*(void) const
{
    return c_str();
}

int hstring::length(void) const
{
    return strlen(c_str());
}

int hstring::handle(void) const
{
    return mHandle;
}
