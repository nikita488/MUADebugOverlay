#pragma once

#include <cstring>
#include "../ratl/hash_pool.h"

#define MAX_HASH 16384
#define	BLOCK_SIZE 102400

typedef	ratl::hash_pool<BLOCK_SIZE, MAX_HASH> TStrPool;

class hstring
{
private:
	int mHandle;

	void init(const char* str);

public:
	hstring();
	hstring(const char* str);
	hstring(const hstring& str);

	hstring& operator=(const char* str);
	hstring& operator=(const hstring& str);

	bool operator==(const hstring& str) const { return mHandle == str.mHandle; }
	bool operator<(const hstring& str) const { return mHandle < str.mHandle; }
	bool operator!() const { return mHandle == 0; }

	const char* c_str(void) const;
	const char* operator*(void) const;

	int length(void) const;
	int handle(void) const;
	bool empty() const { return handle() == 0; }
};

static_assert(sizeof(hstring) == 4, "Invalid structure size of hstring");
static_assert(sizeof(TStrPool) == 167944, "Invalid structure size of TStrPool");