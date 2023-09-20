#pragma once

namespace ratl
{
	template <int SIZE, int SIZE_HANDLES>
	class hash_pool
	{
		int field_0;
		int mHandles[SIZE_HANDLES];
		int mDataAlloc;
		char mData[SIZE];

	public:
		hash_pool()
		{
			clear();
		}

		int size() const
		{
			return mDataAlloc;
		}

		bool empty() const
		{
			return mDataAlloc == 1;
		}

		bool full(int minimum) const
		{
			return (SIZE - mDataAlloc) < minimum;
		}

		void clear()
		{
			mData[0] = 0;
			mDataAlloc = 1;

			for (int i = 0; i < SIZE_HANDLES; i++)
				mHandles[i] = 0;
		}

		int get_handle(const void* data, int datasize)
		{
			return reinterpret_cast<int (__thiscall*)(hash_pool<SIZE, SIZE_HANDLES>*, const void*, int, int)>(0x41C520)(this, data, datasize, 0);
		}

		const void* operator[](int handle) const
		{
			return &mData[mHandles[handle]];
		}
	};
}