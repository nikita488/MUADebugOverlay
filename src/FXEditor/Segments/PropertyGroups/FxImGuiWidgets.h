#pragma once

#include <filesystem>

#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <imgui/imgui.h>

#include "FxImGuiDefines.h"
#include "../SegmentDataTypes.h"

namespace fs = std::filesystem;

class IAssetCache;

struct GroupSection
{
	bool mEnabled;

	inline GroupSection(bool enabled) :
		mEnabled(enabled)
	{
		if (!mEnabled)
			ImGui::BeginDisabled();
	}

	inline ~GroupSection()
	{
		if (!mEnabled)
			ImGui::EndDisabled();
	}
};

typedef int ImGuiFileDialogFlags;

struct ImGuiFileDialogContext
{
	const char* dialogId;
	const char* title;
	const char* filters;
	IGFD::FileDialogConfig config;
	fs::path root;

	inline ImGuiFileDialogContext() :
		dialogId(NULL),
		title(NULL),
		filters(NULL),
		root(fs::current_path())
	{
	}

	inline void OpenDialog()
	{
		ImGuiFileDialog::Instance()->OpenDialog(dialogId, title, filters, config);
	}

	inline void CloseDialog()
	{
		ImGuiFileDialog::Instance()->Close();
	}

	bool Display();

	inline bool IsConfirmed()
	{
		return ImGuiFileDialog::Instance()->IsOk();
	}

	inline std::string GetFilePath()
	{
		return ImGuiFileDialog::Instance()->GetFilePathName();
	}

	inline void SetInitialPath(const char* initialPath, const char* defaultPath = "")
	{
		config.filePathName = initialPath;

		if (config.filePathName.empty())
		{
			config.filePathName = defaultPath;
		}
	}

	inline void AddFlags(ImGuiFileDialogFlags flags)
	{
		config.flags |= flags;
	}

	inline void RemoveFlags(ImGuiFileDialogFlags flags)
	{
		config.flags &= ~flags;
	}

	void SetupDefaultFlags()
	{
		AddFlags(ImGuiFileDialogFlags_HideColumnType);
		AddFlags(ImGuiFileDialogFlags_HideColumnSize);
		AddFlags(ImGuiFileDialogFlags_HideColumnDate);
		AddFlags(ImGuiFileDialogFlags_Modal);
		AddFlags(ImGuiFileDialogFlags_DisablePlaceMode);
		AddFlags(ImGuiFileDialogFlags_DisableThumbnailMode);
		AddFlags(ImGuiFileDialogFlags_DisableQuickPathSelection);
	}

	fs::path RelativePath(const fs::path& input, bool weekly = false);

	bool RelativePath(const char* input, fs::path& relativePath, bool weekly = false);
};

ImGuiFileDialogContext TextureFileDialog(const char* str_id);

ImGuiFileDialogContext ModelFileDialog(const char* str_id);

ImGuiFileDialogContext ShaderFileDialog(const char* str_id);

ImGuiFileDialogContext EffectFileDialog(const char* str_id);

bool ToggleButton(const char* label, bool active, const ImVec2& size = ImVec2(0, 0));

template<typename T>
bool CheckboxFlagsInverseT(const char* label, T* flags, T flags_value);

bool CheckboxFlagsInverse(const char* label, int* flags, int flags_value);
bool CheckboxFlagsInverse(const char* label, unsigned int* flags, unsigned int flags_value);

bool InputAsset(const char* label, char* buf, size_t buf_size, IAssetCache& cache, const char* initialPath, ImGuiFileDialogContext& dlg);

bool DragAngle2(const char* label, float v_rad[2], float v_speed = 1.0F, float v_min = 0.0F, float v_max = 0.0F, const char* format = VALUE_FMT, ImGuiSliderFlags flags = 0);

bool DragRange(const char* label, EDragDataType data_type, float v[2], float v_speed = 1.0F, float v_min = 0.0F, float v_max = 0.0F, const char* format = VALUE_FMT, ImGuiSliderFlags flags = 0);

bool FloatGroup(const char* label, CRangedFloat& flt, CSegmentFloat& param, float speed = 1.0F, float min = 0.0F, float max = 0.0F, const char* format = VALUE_FMT);

bool VectorGroup(const char* label, int handle, CSegmentVector& param, float speed = 1.0F, float min = 0.0F, float max = 0.0F, const char* format = VALUE_FMT, SegmentVectorFlags flags = SegmentVectorFlags_None);

bool RelativeVectorGroup(const char* label, int handle, CSegmentVector& param, unsigned int& relativeFlags, unsigned int relativeMask, float speed = 1.0F, float min = 0.0F, float max = 0.0F, const char* format = VALUE_FMT, SegmentVectorFlags flags = SegmentQuadraticFlags_None);

bool QuadraticGroup(const char* label, int handle, CSegmentQuadratic& param, float speed = 1.0F, float min = 0.0F, float max = 0.0F, const char* format = VALUE_FMT, SegmentQuadraticFlags flags = SegmentQuadraticFlags_None);

bool ColorGroup(const char* label, CFxPrimitiveTemplate* primitive, CSegmentColor& param);
