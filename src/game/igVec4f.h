#pragma once

class igVec4f
{
public:
	float vec[4];

	inline float& operator[](int idx)
	{
		return vec[idx];
	}

	inline float operator[](int idx) const
	{
		return vec[idx];
	}
};