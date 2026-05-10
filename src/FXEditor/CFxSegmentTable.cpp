#include "CFxSegmentTable.h"

#include <algorithm>
#include <numeric>
#include <string>

#include <imgui/imgui.h>

#include <igGapCore.h>
#include <igCore/igStringObj.h>

#include "IFxEditor.h"
#include "FxTemplateHelpers.h"

#include "Segments/IFxSegment.h"

#include "Segments/PropertyGroups/FxImGuiWidgets.h"

template<typename T>
int cmp(T left, T right)
{
	if (left < right) return -1;
	if (left > right) return 1;
	return 0;
}

template<>
int cmp(const char* left, const char* right)
{
	return std::strcmp(left, right);
}

enum SegmentColumnID
{
	SegmentColumnID_Enabled,
	SegmentColumnID_Name,
	SegmentColumnID_Type,
	SegmentColumnID_Segment,
	SegmentColumnID_Delay,
	SegmentColumnID_Count,
	SegmentColumnID_MAX
};

float GetAverage(const CRangedFloat& rangedFloat)
{
	float min, max;
	rangedFloat.Get(&min, &max);
	return (min + max) * 0.5F;
}

int CompareColumn(int colIndex, IFxSegment* segmentA, IFxSegment* segmentB)
{
	CFxPrimitiveTemplate* templateA = segmentA->GetTemplate();
	CFxPrimitiveTemplate* templateB = segmentB->GetTemplate();
	
	switch (colIndex)
	{
	case SegmentColumnID_Enabled:
		return cmp(segmentA->GetEnabled(), segmentB->GetEnabled());
	case SegmentColumnID_Name:
		return cmp(segmentA->GetName(), segmentB->GetName());
	case SegmentColumnID_Type:
	{
		const char* nameA = GetPrimitiveDisplayName(templateA->field_4);
		const char* nameB = GetPrimitiveDisplayName(templateB->field_4);
		return cmp(nameA, nameB);
	}
	case SegmentColumnID_Segment:
		return cmp(segmentA->GetId(), segmentB->GetId());
	case SegmentColumnID_Delay:
	{
		float avgA = GetAverage(templateA->field_8);
		float avgB = GetAverage(templateB->field_8);
		return cmp(avgA, avgB);
	}
	case SegmentColumnID_Count:
	{
		float avgA = GetAverage(templateA->field_16);
		float avgB = GetAverage(templateB->field_16);
		return cmp(avgA, avgB);
	}
	default:
		assert(0);
		return 0;
	}
}

struct SFxSegmentComparator
{
	ImGuiTableSortSpecs* mTableSortSpecs;
	
	SFxSegmentComparator(ImGuiTableSortSpecs* specs) : 
		mTableSortSpecs(specs)
	{
		assert(mTableSortSpecs);
	}
	
	bool operator()(int a, int b) const
	{
		CFxFile& file = TheFxEditor().GetFile();
		
		IFxSegment* segmentA = file.GetSegment(a);
		IFxSegment* segmentB = file.GetSegment(b);

		for (int i = 0; i < mTableSortSpecs->SpecsCount; i++)
		{
			auto* spec = &mTableSortSpecs->Specs[i];
			int result = CompareColumn(spec->ColumnIndex, segmentA, segmentB);

			if (result != 0)
			{
				bool isAscending = spec->SortDirection == ImGuiSortDirection_Ascending;
				return isAscending ? result < 0 : result > 0;
			}
		}

		return segmentA->GetId() < segmentB->GetId();
	}
};

void RangedFloatCell(const CRangedFloat& rangedFloat)
{
	float min, max;
	rangedFloat.Get(&min, &max);

	if (min != max)
	{
		ImGui::Text("%.2f - %.2f", min, max);
	}
	else
	{
		ImGui::Text("%.2f", min);
	}
}

void CFxSegmentTable::SegmentRow(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	{
		GroupSection section(TheFxEditor().GetFilter().Matches(segment));
		
		ImGui::PushID(segment->GetId());
		ImGui::TableNextRow();

		if (ImGui::TableSetColumnIndex(SegmentColumnID_Enabled))
		{
			bool enabled = segment->GetEnabled();

			if (ImGui::Checkbox("##SegmentEnabled", &enabled))
			{
				segment->SetEnabled(enabled);
			}
		}

		if (ImGui::TableSetColumnIndex(SegmentColumnID_Name))
		{
			igStringObjRef label = igStringObj::instantiateRefFromPool(kIGMemoryPoolTemporary);

			const char* name = segment->GetName();

			if (name && name[0])
			{
				label->set(name);
			}
			else
			{
				label->printf("Unnamed %s %d", GetPrimitiveDisplayName(primTemplate->field_4), segment->GetId());
			}

			bool isSelected = segment == TheFxEditor().GetSelectedSegment();

			ImGui::PushID(label->getBuffer());

			if (ImGui::Selectable(label->getBuffer(), isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap))
			{
				TheFxEditor().SetSelectedSegment(segment);
			}

			ImGui::PopID();
		}

		if (ImGui::TableSetColumnIndex(SegmentColumnID_Type))
		{
			const char* name = GetPrimitiveDisplayName(primTemplate->field_4);
			ImGui::TextUnformatted(name);
		}

		if (ImGui::TableSetColumnIndex(SegmentColumnID_Segment))
		{
			ImGui::Text("%d", segment->GetId());
		}

		if (ImGui::TableSetColumnIndex(SegmentColumnID_Delay))
		{
			RangedFloatCell(primTemplate->field_8);
		}

		if (ImGui::TableSetColumnIndex(SegmentColumnID_Count))
		{
			RangedFloatCell(primTemplate->field_16);
		}

		ImGui::PopID();
	}
}

void CFxSegmentTable::ProcessSorting()
{
	int segmentCount = TheFxEditor().GetFile().GetSegmentCount();

	if (mIndices.size() != segmentCount)
	{
		mIndices.resize(segmentCount);
		std::iota(mIndices.begin(), mIndices.end(), 0);

		mDirty = true;
	}
	
	ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();

	if (!sortSpecs)
	{
		return;
	}

	if (sortSpecs->SpecsDirty)
	{
		mDirty = true;
		sortSpecs->SpecsDirty = false;
	}

	if (mDirty)
	{
		if (segmentCount > 1)
		{
			std::sort(mIndices.begin(), mIndices.end(), SFxSegmentComparator(sortSpecs));
		}

		mDirty = false;
	}
}

CFxSegmentTable::CFxSegmentTable() : 
	mDirty(false)
{
}

void CFxSegmentTable::Clear()
{
	mIndices.clear();
	mDirty = false;
}

void CFxSegmentTable::Update()
{
	//TODO: Remove ImGuiTableFlags_NoSavedSettings?
	ImGuiTableFlags flags = ImGuiTableFlags_NoSavedSettings | 
		ImGuiTableFlags_SizingFixedFit | 
		ImGuiTableFlags_Resizable | 
		ImGuiTableFlags_Sortable | 
		ImGuiTableFlags_ContextMenuInBody | 
		ImGuiTableFlags_Borders | 
		ImGuiTableFlags_NoBordersInBody;

	if (ImGui::BeginTable("EffectSegmentsTable", SegmentColumnID_MAX, flags))
	{
		ImGui::TableSetupColumn("##Status", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoHeaderLabel, ImGui::GetFrameHeight());
		ImGui::TableSetupColumn("Name", 0, 200.0F);
		ImGui::TableSetupColumn("Type", 0, 150.0F);
		ImGui::TableSetupColumn("Segment", ImGuiTableColumnFlags_DefaultSort, 80.0F);
		ImGui::TableSetupColumn("Delay", 0, 80.0F);
		ImGui::TableSetupColumn("Count", 0, 80.0F);
		ImGui::TableSetupScrollFreeze(0, 1);

		ProcessSorting();

		ImGui::TableHeadersRow();

		CFxFile& file = TheFxEditor().GetFile();

		for (int index : mIndices)
		{
			SegmentRow(file.GetSegment(index));
		}

		ImGui::EndTable();
	}
}
