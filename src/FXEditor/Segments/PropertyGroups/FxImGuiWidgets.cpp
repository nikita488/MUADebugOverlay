#include "FxImGuiWidgets.h"

#include <assert.h>
#include <string>
#include <functional>
#include <exception>

#include <imgui/imgui_internal.h>

#if !defined(RATL_COMMON_INC)
	#include <Common/Ratl/ratl_common.h>
#endif

#include "../../IFxEditor.h"
#include "../../IAssetCache.h"

bool ImGuiFileDialogContext::Display()
{
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	ImVec2 maxSize = displaySize;
	ImVec2 minSize = ImVec2(displaySize.x * 0.3F, displaySize.y * 0.3F);
	ImVec2 defaultSize = ImVec2(displaySize.x * 0.5F, displaySize.y * 0.5F);

	ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Once);
	ImGui::SetNextWindowSize(defaultSize, ImGuiCond_Once);

	return ImGuiFileDialog::Instance()->Display(dialogId, ImGuiWindowFlags_NoCollapse, minSize, maxSize);
}

fs::path ImGuiFileDialogContext::RelativePath(const fs::path& input, bool weekly)
{
	if (!weekly && !fs::is_regular_file(input))
	{
		throw std::invalid_argument("Input path is not a regular file");
	}

	if (!fs::is_directory(root))
	{
		throw std::invalid_argument("Root path is not a directory");
	}

	fs::path canonicalInput = weekly ? fs::weakly_canonical(input) : fs::canonical(input);
	fs::path canonicalRoot = fs::canonical(root);

	auto [rootEnd, _] = std::mismatch(
		canonicalRoot.begin(), canonicalRoot.end(),
		canonicalInput.begin(), canonicalInput.end()
	);

	if (rootEnd != canonicalRoot.end())
	{
		throw std::runtime_error("Input path is not under root path");
	}

	return fs::relative(canonicalInput);
}

bool ImGuiFileDialogContext::RelativePath(const char* input, fs::path& relativePath, bool weekly)
{
	try
	{
		relativePath = RelativePath(input, weekly);
		return true;
	}
	catch (const std::exception& e)
	{
		TheFxEditor().ShowError(e.what());
		return false;
	}
}

ImGuiFileDialogContext TextureFileDialog(const char* str_id)
{
	ImGuiFileDialogContext ctx;

	ctx.dialogId = str_id;
	ctx.title = "Select Texture file";
	ctx.filters = "Texture (*.igb){.igb}";
	ctx.SetupDefaultFlags();
	return ctx;
}

ImGuiFileDialogContext ModelFileDialog(const char* str_id)
{
	ImGuiFileDialogContext ctx;

	ctx.dialogId = str_id;
	ctx.title = "Select Model file";
	ctx.filters = "Model (*.igb){.igb}";
	ctx.SetupDefaultFlags();
	return ctx;
}

ImGuiFileDialogContext ShaderFileDialog(const char* str_id)
{
	ImGuiFileDialogContext ctx;

	ctx.dialogId = str_id;
	ctx.title = "Select Shader file";
	ctx.filters = "Shader (*.igb){.igb}";
	ctx.SetupDefaultFlags();
	return ctx;
}

ImGuiFileDialogContext EffectFileDialog(const char* str_id)
{
	ImGuiFileDialogContext ctx;

	ctx.dialogId = str_id;
	ctx.title = "Select Effect file";
	ctx.filters = "Effect (*.xmlb){.xmlb}";
	ctx.root = "effects/";
	ctx.SetupDefaultFlags();
	return ctx;
}

bool ToggleButton(const char* label, bool active, const ImVec2& size)
{
	if (active)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
	}

	bool changed = ImGui::Button(label, size);

	if (active)
	{
		ImGui::PopStyleColor();
	}

	return changed;
}

template<typename T>
bool CheckboxFlagsInverseT(const char* label, T* flags, T flags_value)
{
	bool all_off = (*flags & flags_value) == 0;
	bool any_off = (*flags & flags_value) != flags_value;
	bool pressed;
	if (!all_off && any_off)
	{
		ImGuiContext& g = *ImGui::GetCurrentContext();
		g.NextItemData.ItemFlags |= ImGuiItemFlags_MixedValue;
		pressed = ImGui::Checkbox(label, &all_off);
	}
	else
	{
		pressed = ImGui::Checkbox(label, &all_off);

	}
	if (pressed)
	{
		if (all_off)
			*flags &= ~flags_value;
		else
			*flags |= flags_value;
	}
	return pressed;
}

bool CheckboxFlagsInverse(const char* label, int* flags, int flags_value)
{
	return CheckboxFlagsInverseT(label, flags, flags_value);
}

bool CheckboxFlagsInverse(const char* label, unsigned int* flags, unsigned int flags_value)
{
	return CheckboxFlagsInverseT(label, flags, flags_value);
}

typedef std::function<bool ()> TBrowsePressCallback;

bool InputPath(const char* label, char* buf, size_t buf_size, TBrowsePressCallback onBrowsePressed, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (window->SkipItems)
		return false;

	ImGuiStyle& style = ImGui::GetStyle();

	const char* button_label = "Browse";
	const ImVec2 label_size = ImGui::CalcTextSize(button_label, NULL, true);
	const ImVec2 button_size(
		label_size.x + style.FramePadding.x * 2.0f, 
		label_size.y + style.FramePadding.y * 2.0f);
	const float button_space = button_size.x + style.ItemInnerSpacing.x;

	ImGui::PushID(label);
	ImGui::SetNextItemWidth(ImMax(1.0f, ImGui::CalcItemWidth() - button_space));

	bool dirty = false;

	ImGui::InputText("##Path", buf, buf_size, flags);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		dirty = true;
	}

	const ImVec2 backup_frame_padding = style.FramePadding;
	
	style.FramePadding.x = style.FramePadding.y;

	ImGui::SameLine(0, style.ItemInnerSpacing.x);

	if (ImGui::Button(button_label, button_size))
	{
		dirty |= onBrowsePressed();
	}

	const char* label_end = ImGui::FindRenderedTextEnd(label);

	if (label != label_end)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		ImGui::TextEx(label, label_end);
	}

	style.FramePadding = backup_frame_padding;
	ImGui::PopID();
	return dirty;
}

bool InputFile(const char* label, char* buf, size_t buf_size, const char* initialPath, ImGuiFileDialogContext& dlg)
{
	//auto resolvePath = [&dlg, &initialPath](const char* path) -> std::string
	//{
	//	try
	//	{
	//		return dlg.RelativePath(path).generic_string();
	//	}
	//	catch (const std::exception& e)
	//	{
	//		TheFxEditor().ShowError(e.what());
	//		return initialPath;
	//	}
	//};

	auto resolvePath = [&dlg, &initialPath](const char* input) -> std::string
	{
		fs::path relativePath;
		bool success = input[0] == '\0' || dlg.RelativePath(input, relativePath);

		if (!success)
		{
			return initialPath;
		}

		return relativePath.generic_string();
	};

	bool dirty = InputPath(label, buf, buf_size, [&dlg]()
	{
		dlg.OpenDialog();
		return false;
	});

	if (dirty)
	{
		ratl::str::ncpy(buf, resolvePath(buf).c_str(), buf_size);
	}

	if (dlg.Display())
	{
		if (dlg.IsConfirmed())
		{
			std::string fixedPath = resolvePath(dlg.GetFilePath().c_str());
			ratl::str::ncpy(buf, fixedPath.c_str(), buf_size);
			dirty = true;
		}

		dlg.CloseDialog();
	}

	return dirty;
}

void UpdateAssetCache(IAssetCache& cache, const char* oldPath, const char* newPath)
{
	if (oldPath && oldPath[0])
	{
		cache.RemoveAsset(oldPath, MP_MENU);
	}

	if (newPath && newPath[0])
	{
		cache.AddAsset(newPath, MP_MENU);
	}
}

bool InputAsset(const char* label, char* buf, size_t buf_size, IAssetCache& cache, const char* initialPath, ImGuiFileDialogContext& dlg)
{
	bool valueChanged = InputFile(label, buf, buf_size, initialPath, dlg);
	
	if (valueChanged)
	{
		if (!ratl::str::eql(buf, initialPath))
		{
			UpdateAssetCache(cache, initialPath, buf);
			return true;
		}
	}

	return false;
}

bool DragAngle2(const char* label, float v_rad[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	float v_deg[2] =
	{
		igRadiansToDegreesf(v_rad[0]),
		igRadiansToDegreesf(v_rad[1])
	};

	bool changed = ImGui::DragFloat2(label, v_deg, v_speed, v_min, v_max, format, flags);

	if (changed)
	{
		v_rad[0] = igDegreesToRadiansf(v_deg[0]);
		v_rad[1] = igDegreesToRadiansf(v_deg[1]);
	}

	return changed;
}

bool DragRange(const char* label, EDragDataType data_type, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	//TODO: Reimplement this
	if (false && data_type == DDT_ANGLE)
	{
		return DragAngle2(label, v, v_speed, v_min, v_max, format, flags);
	}
	else
	{
		return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, format, flags);
	}
}

bool FloatGroup(const char* label, CRangedFloat& flt, CSegmentFloat& param, float speed, float min, float max, const char* format)
{
	bool valueChanged = false;
	
	ImGui::PushID(label);
	valueChanged = param.Draw(label, flt);
	ImGui::PopID();
	return valueChanged;
}

bool VectorGroup(const char* label, int handle, CSegmentVector& param, float speed, float min, float max, const char* format, SegmentVectorFlags flags)
{
	assert(handle);
	
	bool valueChanged = false;
	
	ImGui::PushID(handle);
	ImGui::SeparatorText(label);
	valueChanged = param.Draw(handle, speed, min, max, format, flags);
	ImGui::PopID();
	return valueChanged;
}

bool RelativeVectorGroup(const char* label, int handle, CSegmentVector& param, unsigned int& relativeFlags, unsigned int relativeMask, float speed, float min, float max, const char* format, SegmentVectorFlags flags)
{
	assert(handle);
	ImGui::PushID(handle);
	ImGui::SeparatorText(label);

	bool valueChanged = false;
	bool isRelative = (relativeFlags & relativeMask) == 0;

	if (isRelative)
	{
		flags |= SegmentVectorFlags_Relative;
	}
	else
	{
		flags &= ~SegmentVectorFlags_Relative;
	}

	valueChanged |= param.Draw(handle, speed, min, max, format, flags);
	
	{
		GroupSection section((flags & SegmentVectorFlags_NoRelative) == 0);
		valueChanged |= CheckboxFlagsInverse("Relative to effect axis", &relativeFlags, relativeMask);
	}

	ImGui::PopID();
	return valueChanged;
}

bool QuadraticGroup(const char* label, int handle, CSegmentQuadratic& param, float speed, float min, float max, const char* format, SegmentQuadraticFlags flags)
{
	assert(handle);

	bool valueChanged = false;

	ImGui::PushID(handle);
	ImGui::SeparatorText(label);
	valueChanged = param.Draw(handle, speed, min, max, format, flags);
	ImGui::PopID();
	return valueChanged;
}

bool ColorGroup(const char* label, CFxPrimitiveTemplate* primitive, CSegmentColor& param)
{
	assert(primitive);

	bool valueChanged = false;

	ImGui::PushID(label);
	ImGui::SeparatorText(label);
	valueChanged = param.Draw(primitive);
	ImGui::PopID();
	return valueChanged;
}
