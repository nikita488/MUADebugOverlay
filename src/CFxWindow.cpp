#include "CFxWindow.h"

#include <imgui/imgui.h>

#include <igGapMath.h>
#include <igMath/igVec4f.h>

#include <FX/CRangedComponentPool.h>
#include <FX/CFxPrimitiveTemplate.h>
#include <FX/CFxTemplateManager.h>
#include <FX/CFxTemplate.h>

using namespace Gap;

const char* CFxWindow::fxPrimitiveNames[16] =
{
	"NULL",//hack for 0 index
	"Empty",
	"ParticleCloud",
	"Sprite",
	"WeatherEffect",
	"OrientedSprite",
	"Spark",
	"Line",
	"Lightning",
	"Cylinder",
	"Model",
	"Light",
	"ScreenFlash",
	"CameraShake",
	"Trail",
	"BezierCurve",
};

const char* CFxWindow::fxBlendNames[5] =
{
	"alpha",
	"additive",
	"subtractive",
	"alphaadditive",
	"source",
};

void AddRangeInput(const char* label, float* range)
{
	float max = range[0] + range[1];
	ImVec2 fixedRange(range[0], max);

	if (ImGui::InputFloat2(label, (float*)&fixedRange, "%g"))
	{
		range[0] = fixedRange[0];
		range[1] = fixedRange[1] - fixedRange[0];
	}
}

void AddColorPicker(const char* label, unsigned int& abgr)
{
	igVec4f color;
	color.unpackColor(IG_MATH_COLORSPACE_FORMAT_RGBA, abgr, 0.0F);//wtf Alchemy

	bool changed = ImGui::ColorEdit4(label, (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

	if (changed)
	{
		abgr = color.packColor(IG_MATH_COLORSPACE_FORMAT_RGBA);
	}
}

void AddQuadComponent(const char* label, short handle)
{
	CRangedQuadratic& component = TheQuadPool().field_0[handle];

	if (ImGui::TreeNode(label))
	{
		float aMax = component._base._a + component._range[0];
		float bMax = component._base._b + component._range[1];
		float cMax = component._base._c + component._range[2];
		ImVec2 a(component._base._a, aMax);
		ImVec2 b(component._base._b, bMax);
		ImVec2 c(component._base._c, cMax);
		ImVec2 range(component._base._min, component._base._max);

		ImGui::Text("Quadratic component");

		if (handle >= 0 && handle <= 150) //0 - default, 1-150 - defaultquads
		{
			if (handle == 0)
			{
				ImGui::Text("Uses Default Values");
			}
			else
			{
				ImGui::Text("Uses Default Quad %d", handle);
			}

			ImGui::BeginDisabled();
		}

		if (ImGui::InputFloat2("a Min/Max##aRange", (float*)&a, "%g"))
		{
			component._base._a = a[0];
			component._range[0] = a[1] - a[0];
		}

		if (ImGui::InputFloat2("b Min/Max##bRange", (float*)&b, "%g"))
		{
			component._base._b = b[0];
			component._range[1] = b[1] - b[0];
		}

		if (ImGui::InputFloat2("c Min/Max##cRange", (float*)&c, "%g"))
		{
			component._base._c = c[0];
			component._range[2] = c[1] - c[0];
		}

		if (ImGui::InputFloat2("Clamp Min/Max##clampRange", (float*)&range, "%g"))
		{
			component._base._min = range[1];
			component._base._max = range[0];
		}

		if (handle >= 0 && handle <= 150)
		{
			ImGui::EndDisabled();
		}

		ImGui::TreePop();
	}
}

void AddVectorComponent(const char* label, short handle)
{
	CRangedVector& component = TheVectorPool().field_0[handle];

	if (ImGui::TreeNode(label))
	{
		ImGui::Text("Vector component");

		float xMax = component._base[0] + component._range[0];
		float yMax = component._base[1] + component._range[1];
		float zMax = component._base[2] + component._range[2];
		ImVec2 x(component._base[0], xMax);
		ImVec2 y(component._base[1], yMax);
		ImVec2 z(component._base[2], zMax);

		if (handle >= 0 && handle <= 85) //0 - default, 1-85 - defaultvectors
		{
			if (handle == 0)
			{
				ImGui::Text("Uses Default Values");
			}
			else
			{
				ImGui::Text("Uses Default Vector %d", handle);
			}

			ImGui::BeginDisabled();
		}

		if (ImGui::InputFloat2("X Min/Max##xRange", (float*)&x, "%g"))
		{
			component._base[0] = x[0];
			component._range[0] = x[1] - x[0];
		}

		if (ImGui::InputFloat2("Y Min/Max##yRange", (float*)&y, "%g"))
		{
			component._base[1] = y[0];
			component._range[1] = y[1] - y[0];
		}

		if (ImGui::InputFloat2("Z Min/Max##zRange", (float*)&z, "%g"))
		{
			component._base[2] = z[0];
			component._range[2] = z[1] - z[0];
		}

		if (handle >= 0 && handle <= 85)
		{
			ImGui::EndDisabled();
		}

		ImGui::TreePop();
	}
}

template<int ARG_CAPACITY>
void AddInputString(const char* label, ratl::string_vs<ARG_CAPACITY>& str)
{
	char name[ARG_CAPACITY];
	strncpy(name, *str, ARG_CAPACITY);

	if (ImGui::InputText(label, name, ARG_CAPACITY))
	{
		str = name;
	}
}

void AddFlags(const char* label, unsigned int& flags)
{
	if (ImGui::TreeNode(label))
	{
		ImGui::InputScalar("##flagsInput", ImGuiDataType_U32, &flags, NULL, NULL, "%u");

		if (ImGui::BeginTable("flagsSplit", 4))
		{
			for (unsigned int i = 0; i < 32; i++)
			{
				char label[64];
				snprintf(label, 64, "%d", i);
				ImGui::TableNextColumn();
				ImGui::CheckboxFlags(label, &flags, 1 << i);
			}
			ImGui::EndTable();
		}

		ImGui::TreePop();
	}
}

void AddFxPrimitiveTemplate(CFxPrimitiveTemplate& primitiveTemplate)
{
	int& primitiveIndex = primitiveTemplate.field_4;

	if (ImGui::BeginCombo("Primitive Type", CFxWindow::fxPrimitiveNames[primitiveIndex]))
	{
		for (int i = 1; i < IM_ARRAYSIZE(CFxWindow::fxPrimitiveNames); i++)
		{
			bool isSelected = i == primitiveIndex;

			if (ImGui::Selectable(CFxWindow::fxPrimitiveNames[i], isSelected))
			{
				primitiveIndex = i;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	AddRangeInput("delay Min/Max", &primitiveTemplate.field_8);
	AddRangeInput("count Min/Max", &primitiveTemplate.field_16);
	AddRangeInput("life Min/Max", &primitiveTemplate.field_24);
	AddRangeInput("drag Min/Max", &primitiveTemplate.field_32);
	AddRangeInput("gravity Min/Max", &primitiveTemplate.field_40);
	AddFlags("primitiveFlags", primitiveTemplate.field_48);
	AddFlags("primitiveFlags2", primitiveTemplate.field_52);
	AddFlags("spawnFlags", primitiveTemplate.field_56);
	ImGui::Text("DeathFxFile: %d", primitiveTemplate.field_60);//check
	ImGui::Text("IntervalFxFile: %d", primitiveTemplate.field_64);//check
	ImGui::InputFloat("Interval", &primitiveTemplate.field_68, 0.0F, 0.0F, "%g");
	ImGui::InputFloat("viewoffset", &primitiveTemplate.field_72, 0.0F, 0.0F, "%g");
	AddRangeInput("radius Min/Max", &primitiveTemplate.field_76);
	AddRangeInput("radius2 Min/Max", &primitiveTemplate.field_84);
	AddRangeInput("height Min/Max", &primitiveTemplate.field_92);

	AddColorPicker("startColor1Picker", primitiveTemplate.field_100); ImGui::SameLine();
	ImGui::InputScalar("startColor1", ImGuiDataType_U32, &primitiveTemplate.field_100, NULL, NULL, "%u");

	AddColorPicker("midColor1Picker", primitiveTemplate.field_104); ImGui::SameLine();
	ImGui::InputScalar("midColor1", ImGuiDataType_U32, &primitiveTemplate.field_104, NULL, NULL, "%u");

	AddColorPicker("endColor1Picker", primitiveTemplate.field_108); ImGui::SameLine();
	ImGui::InputScalar("endColor1", ImGuiDataType_U32, &primitiveTemplate.field_108, NULL, NULL, "%u");

	AddColorPicker("startColor2Picker", primitiveTemplate.field_112); ImGui::SameLine();
	ImGui::InputScalar("startColor2", ImGuiDataType_U32, &primitiveTemplate.field_112, NULL, NULL, "%u");

	AddColorPicker("midColor2Picker", primitiveTemplate.field_116); ImGui::SameLine();
	ImGui::InputScalar("midColor2", ImGuiDataType_U32, &primitiveTemplate.field_116, NULL, NULL, "%u");

	AddColorPicker("endColor2Picker", primitiveTemplate.field_120); ImGui::SameLine();
	ImGui::InputScalar("endColor2", ImGuiDataType_U32, &primitiveTemplate.field_120, NULL, NULL, "%u");

	int& blendIndex = primitiveTemplate.field_124;

	if (ImGui::BeginCombo("blend", CFxWindow::fxBlendNames[blendIndex]))
	{
		for (int i = 0; i < IM_ARRAYSIZE(CFxWindow::fxBlendNames); i++)
		{
			bool isSelected = i == blendIndex;

			if (ImGui::Selectable(CFxWindow::fxBlendNames[i], isSelected))
			{
				blendIndex = i;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	AddRangeInput("drag2 Min/Max", &primitiveTemplate.field_128);
	AddRangeInput("gravity2 Min/Max", &primitiveTemplate.field_136);
	ImGui::InputFloat("pLife", &primitiveTemplate.field_144, 0.0F, 0.0F, "%g");
	AddRangeInput("ptravel Min/Max", &primitiveTemplate.field_148);
	AddRangeInput("pspawn Min/Max", &primitiveTemplate.field_156);
	ImGui::InputFloat("shakespeed", &primitiveTemplate.field_164, 0.0F, 0.0F, "%g");
	ImGui::InputFloat("shakescale", &primitiveTemplate.field_168, 0.0F, 0.0F, "%g");
	ImGui::InputInt("shaketype", &primitiveTemplate.field_172);

	AddVectorComponent("origin", primitiveTemplate.field_176);
	AddVectorComponent("velocity", primitiveTemplate.field_178);
	AddVectorComponent("acceleration", primitiveTemplate.field_180);
	AddQuadComponent("size", primitiveTemplate.field_182);
	AddQuadComponent("alpha", primitiveTemplate.field_184);
	AddQuadComponent("length", primitiveTemplate.field_186);
	AddQuadComponent("offset", primitiveTemplate.field_188);
	AddQuadComponent("rotation", primitiveTemplate.field_190);
	AddQuadComponent("rotationRadius", primitiveTemplate.field_192);
	AddVectorComponent("rotationAxis", primitiveTemplate.field_194);
	AddVectorComponent("orientAxis", primitiveTemplate.field_196);
	AddQuadComponent("chaos", primitiveTemplate.field_198);
	AddQuadComponent("attenuation", primitiveTemplate.field_200);
	AddQuadComponent("startarc", primitiveTemplate.field_202);
	AddQuadComponent("endarc", primitiveTemplate.field_204);
	AddQuadComponent("size2", primitiveTemplate.field_206);
	AddVectorComponent("origin2", primitiveTemplate.field_208);
	AddVectorComponent("velocity2", primitiveTemplate.field_210);
	AddVectorComponent("acceleration2", primitiveTemplate.field_212);
	AddQuadComponent("uvscroll", primitiveTemplate.field_214);
	AddQuadComponent("uvscale", primitiveTemplate.field_216);
	int step = 1;
	ImGui::InputScalar("numsegments", ImGuiDataType_S16, &primitiveTemplate.field_218, (void*)&step, (void*)&step);
	AddVectorComponent("transformRotation", primitiveTemplate.field_220);
	AddVectorComponent("startVec", primitiveTemplate.field_222);
	AddVectorComponent("endVec", primitiveTemplate.field_224);

	static const char* shaderTextures[6] =
	{
		"shadertexture0",
		"shadertexture1",
		"shadertexture2",
		"shadertexture3",
		"shadertexture4",
		"shadertexture5",
	};

	for (int i = 0; i < primitiveTemplate.field_226.SIZE; i++)
	{
		ratl::string_vs<64>& textureName = primitiveTemplate.field_226[i];
		AddInputString<64>(shaderTextures[i], textureName);
	}

	AddInputString<64>("shader", primitiveTemplate.field_616);
	AddFlags("shaderFlags", primitiveTemplate.field_684);
	AddInputString<64>("modelName", primitiveTemplate.field_688);
}

void CFxWindow::Draw()
{
	if (ImGui::Begin("FX Template"))
	{
		CFxTemplateManager& fxTemplateManager = TheFxTemplateManager();

		for (auto it = fxTemplateManager.field_0.begin(); it != fxTemplateManager.field_0.end(); it++)
		{
			if (ImGui::TreeNode(it.key().field_0.c_str()))
			{
				CFxTemplate& fxTemplate = fxTemplateManager.field_19416[it.value()];

				ImGui::InputFloat("LoopTime", &fxTemplate.field_4, 0.0F, 0.0F, "%g");
				ImGui::InputFloat("RandLoopTime", &fxTemplate.field_8, 0.0F, 0.0F, "%g");
				ImGui::Checkbox("PersistLoop", &fxTemplate.field_12);
				ImGui::Text("Memory Pool ID: %d", fxTemplate.field_16);

				int index = 0;

				for (CFxPrimitiveTemplate* primitiveTemplatePtr = fxTemplate.field_0; primitiveTemplatePtr; primitiveTemplatePtr = primitiveTemplatePtr->field_0)
				{
					CFxPrimitiveTemplate& primitiveTemplate = *primitiveTemplatePtr;

					char label[64];
					snprintf(label, 64, "%s##%d", fxPrimitiveNames[primitiveTemplate.field_4], index);

					if (ImGui::TreeNode(label))
					{
						AddFxPrimitiveTemplate(primitiveTemplate);
						ImGui::TreePop();
					}

					index++;
				}

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}