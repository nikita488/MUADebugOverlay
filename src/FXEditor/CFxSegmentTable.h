#pragma once

#include <vector>

class IFxSegment;

class CFxSegmentTable
{
private:
	std::vector<int> mIndices;
	bool mDirty;

private:
	void SegmentRow(IFxSegment* segment);
	void ProcessSorting();

public:
	CFxSegmentTable();

	void Clear();
	void Update();

	inline void MarkDirty()
	{
		mDirty = true;
	}
};

