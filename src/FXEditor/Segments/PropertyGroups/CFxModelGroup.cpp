#include "CFxModelGroup.h"

#include <imgui/imgui.h>

#include "../../IFxEditor.h"
#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

const char* CFxModelGroup::GetName() const
{
	return "Model";
}

void CFxModelGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	const ratl::string_vs<64>& modelName = primTemplate->field_688;
	bool isDefaultValue = modelName == "textures/default.png";

	if (!isDefaultValue)
	{
		mModel = modelName.c_str();
	}
}

bool CFxModelGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& flags = primTemplate->field_48;
	unsigned int& flags2 = primTemplate->field_52;

	bool valueChanged = false;

	IAssetCache& modelCache = TheFxEditor().ModelCache();
	ImGuiFileDialogContext ctx = ModelFileDialog("ModelFileDlg");
	const char* initialPath = !mModel.empty() ? primTemplate->field_688.c_str() : "";

	ctx.SetInitialPath(initialPath, "models/");

	if (InputAsset("Model", mModel.c_str(), mModel.capacity(), modelCache, primTemplate->field_688.c_str(), ctx))
	{
		primTemplate->field_688 = mModel;
		valueChanged = true;
	}

	ImGui::SeparatorText("Options");

	valueChanged |= ImGui::CheckboxFlags("Enable physics", &flags, 0x200);
	valueChanged |= ImGui::CheckboxFlags("Enable random spin", &flags, 0x8000);
	valueChanged |= CheckboxFlagsInverse("Apply rotation", &flags2, 0x400);

	ImGui::SeparatorText("Render Options");

	valueChanged |= CheckboxFlagsInverse("Enable lighting", &flags, 0x1);
	valueChanged |= ImGui::CheckboxFlags("Apply color tint", &flags, 0x40000);

	return valueChanged;
}
