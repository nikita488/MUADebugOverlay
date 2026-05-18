#include "CFxColorGroup.h"

#include <imgui/imgui.h>
#include <Misc/Common.h>

#include "FxImGuiWidgets.h"
#include "FxPropertyHelpers.h"

#include "../../IFxEditor.h"

#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <filesystem>

CFxColorGroup::CFxColorGroup() : 
	mApplyShader(false),
	mShaderType(SHADER_LIT),
	mTurnUV(false)
{
}

const char* CFxColorGroup::GetName() const
{
	return "Material";
}

void CFxColorGroup::Initialize(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	unsigned int flags = primTemplate->field_48;
	unsigned int flags2 = primTemplate->field_52;
	unsigned int shaderFlags = primTemplate->field_684;
	
	mColor.Set(primTemplate);
	mAlpha.Set(primTemplate->field_184);

	mTexture = primTemplate->field_688;
	
	for (int i = 0; i < primTemplate->field_226.SIZE; i++)
	{
		mShaderTextures[i] = primTemplate->field_226[i];
	}

	mUVScale.Set(primTemplate->field_216);
	mUVScroll.Set(primTemplate->field_214);

	if (IsProcPrimitive(primTemplate->field_4))
	{
		mTurnUV = (flags & 0x40000) != 0;
	}

	if ((shaderFlags & 0x1) != 0)
	{
		mApplyShader = true;
		mShaderType = SHADER_POSTFX;
	}
	else if ((flags2 & 0x1000000) != 0)
	{
		mApplyShader = true;
		mShaderType = SHADER_LIT;
	}
	else
	{
		mShader = primTemplate->field_616;

		if (!mShader.empty() && !mShaderTextures[0].empty())
		{
			//mApplyShader = true;//TODO: Implement in the far future
			mShaderType = SHADER_CUSTOM;
		}
	}
}

const char* litTextureLabels[MAX_TEXTURES] =
{
	"Diffuse",
	"Normal",
	"Specular",
	"Reflectance",
	"Reflectance Mask",
	"Emissive",
};

const char* postFXTextureLabels[MAX_TEXTURES] =
{
	"Mask",
	"Normal",
	"Texture 2",
	"Texture 3",
	"Texture 4",
	"Texture 5",
};

const char* customTextureLabels[MAX_TEXTURES] =
{
	"Texture 0",
	"Texture 1",
	"Texture 2",
	"Texture 3",
	"Texture 4",
	"Texture 5",
};

const char* shaderTexFileDlgIds[MAX_TEXTURES] =
{
	"ShaderTexture0FileDlg",
	"ShaderTexture1FileDlg",
	"ShaderTexture2FileDlg",
	"ShaderTexture3FileDlg",
	"ShaderTexture4FileDlg",
	"ShaderTexture5FileDlg",
};

bool BlendCombo(int& blendType)
{
	static const char* fxBlendNames[MAX_BLEND_TYPES] =
	{
		"Alpha",
		"Additive",
		"Subtractive",
		"Alpha Additive",
		"Source",
	};

	return ImGui::Combo("Blend", &blendType, fxBlendNames, MAX_BLEND_TYPES);
}

const char* CFxColorGroup::ShaderTextureName(int unitId)
{
	switch (mShaderType)
	{
	case SHADER_LIT:
		return litTextureLabels[unitId];
	case SHADER_POSTFX:
		return postFXTextureLabels[unitId];
	case SHADER_CUSTOM:
	default:
		return customTextureLabels[unitId];
	}
}

bool CFxColorGroup::Draw(IFxSegment* segment)
{
	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();
	EFXPrimType primType = primTemplate->field_4;

	unsigned int& flags = primTemplate->field_48;
	unsigned int& flags2 = primTemplate->field_52;
	unsigned int& shaderFlags = primTemplate->field_684;

	TAssetString& shader = primTemplate->field_616;

	bool valueChanged = false;
	bool flagsChanged = false;
	
	IAssetCache& textureCache = TheFxEditor().TextureCache();
	//TODO: Implement in the far future
	IAssetCache& shaderCache = TheFxEditor().TextureCache();//TheFxEditor().ShaderCache();

	{
		GroupSection section(IsTextureSupported(primType) && !mApplyShader);
		ImGuiFileDialogContext ctx = TextureFileDialog("TextureFileDlg");

		ctx.SetInitialPath(primTemplate->field_688.c_str(), "textures/");
	
		if (InputAsset("Texture", mTexture.c_str(), mTexture.capacity(), textureCache, primTemplate->field_688.c_str(), ctx))
		{
			if (mTexture.empty())
			{
				mTexture = "textures/default.png";
			}
			
			primTemplate->field_688 = mTexture;
			valueChanged = true;
		}
	}

	ImGui::SeparatorText("Shader");

	{
		GroupSection sectionA(IsShaderSupported(primType));

		flagsChanged |= ImGui::Checkbox("Apply shader", &mApplyShader);

		{
			GroupSection sectionB(mApplyShader);
			
			ImGui::SameLine();
			flagsChanged |= ImGui::RadioButton("Lit", &mShaderType, SHADER_LIT); ImGui::SameLine();
			flagsChanged |= ImGui::RadioButton("Post FX", &mShaderType, SHADER_POSTFX);// ImGui::SameLine();
			//TODO: Implement in the far future
			//flagsChanged |= ImGui::RadioButton("Custom", &mShaderType, SHADER_CUSTOM);

			//{
			//	GroupSection sectionC(mShaderType == SHADER_CUSTOM);
			//	ImGuiFileDialogContext ctx = ShaderFileDialog("ShaderFileDlg");
			//
			//	ctx.SetInitialPath(shader.c_str(), "shaders/");
			//
			//	if (InputAsset("Shader", mShader.c_str(), mShader.capacity(), shaderCache, shader.c_str(), ctx))
			//	{
			//		shader = mShader;
			//		valueChanged = true;
			//	}
			//}

			for (int unitId = 0; unitId < primTemplate->field_226.SIZE; unitId++)
			{
				if (mShaderType == SHADER_POSTFX && unitId > 1)
				{
					ImGui::BeginDisabled();
				}

				TAssetString& texture = mShaderTextures[unitId];
				const char* label = ShaderTextureName(unitId);
				ImGuiFileDialogContext ctx = TextureFileDialog(shaderTexFileDlgIds[unitId]);

				ctx.SetInitialPath(primTemplate->field_226[unitId].c_str(), "textures/");

				if (InputAsset(label, texture.c_str(), texture.capacity(), textureCache, primTemplate->field_226[unitId].c_str(), ctx))
				{
					primTemplate->field_226[unitId] = texture;
					valueChanged = true;
				}

				if (mShaderType == SHADER_POSTFX && unitId > 1)
				{
					ImGui::EndDisabled();
				}
			}
		}
	}

	ImGui::SeparatorText("UV Transform");

	{
		GroupSection section(IsProcPrimitive(primType));
		flagsChanged |= ImGui::Checkbox("Turn UV", &mTurnUV);
	}

	{
		GroupSection section(IsUVScaleSupported(primType));
		valueChanged |= QuadraticGroup("UV Scale", primTemplate->field_216, mUVScale);
	}

	{
		GroupSection section(IsUVScrollSupported(primType));
		valueChanged |= QuadraticGroup("UV Scroll", primTemplate->field_214, mUVScroll);
	}

	{
		GroupSection section(IsBlendSupported(primType));

		ImGui::SeparatorText("Blending");
		valueChanged |= BlendCombo(primTemplate->field_124);
	}

	{
		GroupSection section(IsColorSupported(primType));
		valueChanged |= ColorGroup("RGB Color", primTemplate, mColor);
		valueChanged |= ImGui::CheckboxFlags("Modulate RGB value using alpha value", &flags, 0x100);
	}

	{
		GroupSection section(IsAlphaSupported(primType));
		valueChanged |= QuadraticGroup("Alpha", primTemplate->field_184, mAlpha);
	}

	if (flagsChanged)
	{
		if (mTurnUV)
		{
			flags |= 0x40000;
		}
		else
		{
			flags &= ~0x40000;
		}

		flags2 &= ~0x1000000;
		shaderFlags &= ~0x1;
		shader = "";

		if (mApplyShader)
		{
			if (mShaderType == SHADER_LIT)
			{
				shader = "*LIT";
				flags2 |= 0x1000000;
			}
			else if (mShaderType == SHADER_POSTFX)
			{
				shader = "*VideoPost";
				shaderFlags |= 0x1;
			}
			else if (mShaderType == SHADER_CUSTOM)
			{
				shader = mShader;
			}
		}
	}

	valueChanged |= flagsChanged;

	return valueChanged;
}