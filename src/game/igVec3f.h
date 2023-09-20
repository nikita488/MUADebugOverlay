#pragma once

class igVec3f
{
public:
	float vec[3];

	inline float& operator[](int idx)
	{
		return vec[idx];
	}

	inline float operator[](int idx) const
	{
		return vec[idx];
	}
};