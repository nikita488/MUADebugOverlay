#include <Windows.h>

#include "patcher.h"

StaticPatcher *StaticPatcher::ms_head;

StaticPatcher::StaticPatcher(Patcher func) : m_func(func)
{
	m_next = ms_head;
	ms_head = this;
}

void StaticPatcher::Apply()
{
	StaticPatcher *current = ms_head;

	while (current)
	{
		current->Run();
		current = current->m_next;
	}

	ms_head = NULL;
}

volatile uintptr_t patcher_arg;

static DWORD protect[2];

int Unprotect_internal(void *address, size_t size)
{
	return VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &protect[0]);
}

int Protect_internal(void *address, size_t size)
{
	return VirtualProtect(address, size, protect[0], &protect[1]);
}

int GetBaseAddress()
{
	static int addr = reinterpret_cast<int>(GetModuleHandleA(NULL));
	return addr;
}

int GetGlobalAddress(int address)
{
	return GetBaseAddress() - 0x400000 + address;
}

const int GetExternalAddress(const char* processName, int shift, int address)
{
	static int addr = reinterpret_cast<int>(GetModuleHandleA(processName));
	return (GetBaseAddress() - 0x400000) + (addr - shift + address);
}
