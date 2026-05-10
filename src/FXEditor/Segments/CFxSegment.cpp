#include "CFxSegment.h"

#include <imgui/imgui.h>

#include <Misc/Common.h>
#include <Misc/ResourceSpecifier.h>

#include <FX/CFxTemplateManager.h>
#include <FX/CRangedComponentPool.h>

#include "../IFxEditor.h"

#include "PropertyGroups/FxPropertyHelpers.h"
#include "PropertyGroups/CFxGenerationGroup.h"
#include "PropertyGroups/CFxFlagsGroup.h"

CFxSegment::CFxSegment() :
	mId(-1),
	mName(""),
	mUsePlatformOverrides(false),
	mPlatformFlags(0),
	mEnabled(true),
	mTemplate(NULL),
	mResetSelectedTab(true)
{
}

CFxSegment::~CFxSegment()
{
	Uncache();
	
	for (IFxPropertyGroup* group : mPropertyGroups)
	{
		delete group;
	}

	mPropertyGroups.clear();
}

bool UpdateAssetCache(IAssetCache& cache, const char* assetPath, bool uncache = false)
{
	if (assetPath && assetPath[0])
	{
		if (uncache)
		{
			return cache.RemoveAsset(assetPath, MP_MENU);
		}
		else
		{
			return cache.AddAsset(assetPath, MP_MENU);
		}
	}

	return false;
}

void CFxSegment::UpdateAssetCaches(bool uncache)
{
	assert(mTemplate);
	
	IAssetCache& textureCache = TheFxEditor().TextureCache();
	IAssetCache& modelCache = TheFxEditor().ModelCache();
	
	EFXPrimType primType = mTemplate->field_4;

	if (primType == FXPRIM_TYPE_MODEL)
	{
		UpdateAssetCache(modelCache, mTemplate->field_688.c_str(), uncache);
	}
	else if (primType != FXPRIM_TYPE_EMPTY &&
		primType != FXPRIM_TYPE_LIGHT &&
		primType != FXPRIM_TYPE_CAMERA_SHAKE)
	{
		UpdateAssetCache(textureCache, mTemplate->field_688.c_str(), uncache);

		for (int i = 0; i < mTemplate->field_226.SIZE; i++)
		{
			UpdateAssetCache(textureCache, mTemplate->field_226[i].c_str(), uncache);
		}
	}

	if (primType != FXPRIM_TYPE_CAMERA_SHAKE)
	{
		IAssetCache& fxCache = TheFxEditor().FxCache();
		
		UpdateAssetCache(fxCache, mDeathFxFile.c_str(), uncache);
		UpdateAssetCache(fxCache, mIntervalFxFile.c_str(), uncache);

		mTemplate->field_60 = TheFxTemplateManager().Find(mDeathFxFile.c_str(), MP_MENU);
		mTemplate->field_64 = TheFxTemplateManager().Find(mIntervalFxFile.c_str(), MP_MENU);
	}
}

void CFxSegment::SetAttribute(tinyxml2::XMLElement* element, const char* name, const char* value)
{
	assert(mTemplate);

	if (value[0] && IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		element->SetAttribute(name, value);
	}
}

void CFxSegment::SetAttribute(tinyxml2::XMLElement* element, const char* name, int value)
{
	assert(mTemplate);

	if (IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		element->SetAttribute(name, value);
	}
}

void CFxSegment::SetAttribute(tinyxml2::XMLElement* element, const char* name, float value)
{
	assert(mTemplate);

	if (IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		ratl::string_vs<128> str;
		snprintf(str.c_str(), str.capacity(), "%g", value);
		element->SetAttribute(name, str.c_str());
	}
}

void CFxSegment::SetAttribute(tinyxml2::XMLElement* element, const char* name, unsigned int value)
{
	assert(mTemplate);

	if (IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		element->SetAttribute(name, value);
	}
}

void CFxSegment::SetAttribute(tinyxml2::XMLElement* element, const char* name, CRangedFloat& value)
{
	assert(mTemplate);
	
	if (IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		float min = value._base;
		float max = min + value._range;

		ratl::string_vs<128> str;
		snprintf(str.c_str(), str.capacity(), "%g %g", min, max);
		element->SetAttribute(name, str.c_str());
	}
}

void CFxSegment::SetAttribute(tinyxml2::XMLElement* element, const char* name, CRangedVector& value)
{
	assert(mTemplate);

	if (IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		igVec3f& min = value._base;
		igVec3f max = min + value._range;

		ratl::string_vs<128> str;
		snprintf(str.c_str(), str.capacity(), "%g %g %g %g %g %g", min[0], min[1], min[2], max[0], max[1], max[2]);
		element->SetAttribute(name, str.c_str());
	}
}

void CFxSegment::SetAttribute(tinyxml2::XMLElement* element, const char* name, CRangedQuadratic& value)
{
	assert(mTemplate);

	if (IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		igVec3f min(value._base._a, value._base._b, value._base._c);
		igVec3f max = min + value._range;
		float clampMin = value._base._min;
		float clampMax = value._base._max;

		bool isDefaultClamp = clampMin == -10000.0F && clampMax == 10000.0F;

		ratl::string_vs<128> str;

		if (isDefaultClamp)
		{
			snprintf(str.c_str(), str.capacity(), "%g %g %g %g %g %g", min[0], min[1], min[2], max[0], max[1], max[2]);
		}
		else
		{
			snprintf(str.c_str(), str.capacity(), "%g %g %g %g %g %g %g %g", min[0], min[1], min[2], max[0], max[1], max[2], clampMax, clampMin);
		}
		
		element->SetAttribute(name, str.c_str());
	}
}

void CFxSegment::SetRangedVectorAttribute(tinyxml2::XMLElement* element, const char* name, int handle)
{
	assert(mTemplate);
	SetAttribute(element, name, TheVectorPool().Get(handle));
}

void CFxSegment::SetRangedQuadAttribute(tinyxml2::XMLElement* element, const char* name, int handle)
{
	assert(mTemplate);
	SetAttribute(element, name, TheQuadPool().Get(handle));
}

void CFxSegment::SetTextureAttribute(tinyxml2::XMLElement* element, const char* name, const ratl::string_vs<64>& texture)
{
	assert(mTemplate);
	
	if (!texture.empty() && IsPrimitiveAttributeSupported(name, mTemplate->field_4))
	{
		ratl::string_vs<64> texturePath(texture);
		char* buf = texturePath.c_str();
		int len = texturePath.length();

		if (len >= 4 && buf[len - 4] == '.')
		{
			buf[len - 4] = '\0';
		}

		const char* fixedPath = Filename_FixFilename(buf, NULL, ".png");

		element->SetAttribute(name, fixedPath);
	}
}

void CFxSegment::Initialize(CFxPrimitiveTemplate* primitive)
{
	assert(primitive);

	mTemplate = primitive;

	AddPropertyGroup<CFxGenerationGroup>();
	InitializePropertyGroups();

	for (IFxPropertyGroup* group : mPropertyGroups)
	{
		group->Initialize(this);
	}

	Cache();
}

void CFxSegment::InitializePropertyGroups()
{
#ifdef _DEBUG
	AddPropertyGroup<CFxFlagsGroup>();
#endif
}

void CFxSegment::Cache()
{
	UpdateAssetCaches();
}

void CFxSegment::Uncache()
{
	UpdateAssetCaches(true);
}

void CFxSegment::Reset()
{
	mResetSelectedTab = true;
}

void CFxSegment::Clone(IFxSegment* segment)
{
	mName = segment->GetName();
	mEnabled = segment->GetEnabled();
	mUsePlatformOverrides = segment->GetUsePlatformOverrides();
	mPlatformFlags = segment->GetPlatformFlags();
	mDeathFxFile = segment->GetDeathFxFile();
	mIntervalFxFile = segment->GetIntervalFxFile();
}

void CFxSegment::Draw()
{
	bool changed = false;
	
	if (ImGui::BeginTabBar("SegmentTabBar"))
	{
		for (IFxPropertyGroup* group : mPropertyGroups)
		{
			ImGuiTabItemFlags flags = ImGuiTabItemFlags_None;
			
			if (mResetSelectedTab)
			{
				flags |= ImGuiTabItemFlags_SetSelected;
				mResetSelectedTab = false;
			}

			if (ImGui::BeginTabItem(group->GetName(), NULL, flags))
			{
				changed |= group->Draw(this);
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	if (changed)
	{
		TheFxEditor().GetFile().MarkDirty();
	}
}

const char* fxBlendValues[5] =
{
	"alpha",
	"additive",
	"subtractive",
	"alphaadditive",
	"source",
};

void CFxSegment::SetAttributes(tinyxml2::XMLElement* element)
{
	assert(mTemplate);
	
	SetRangedVectorAttribute(element, "acceleration", mTemplate->field_180);
	SetRangedVectorAttribute(element, "acceleration2", mTemplate->field_212);
	SetRangedQuadAttribute(element, "alpha", mTemplate->field_184);
	SetRangedQuadAttribute(element, "attenuation", mTemplate->field_200);
	SetAttribute(element, "blend", fxBlendValues[mTemplate->field_124]);
	SetRangedQuadAttribute(element, "chaos", mTemplate->field_198);
	SetAttribute(element, "count", mTemplate->field_16);
	SetAttribute(element, "deathfxfile", mDeathFxFile.c_str());//TODO: Preprocess and remove extension
	SetAttribute(element, "delay", mTemplate->field_8);
	SetAttribute(element, "drag", mTemplate->field_32);
	SetAttribute(element, "drag2", mTemplate->field_128);
	SetRangedQuadAttribute(element, "endarc", mTemplate->field_204);
	SetAttribute(element, "endcolor1", mTemplate->field_108);
	SetAttribute(element, "endcolor2", mTemplate->field_120);
	SetRangedVectorAttribute(element, "endvec", mTemplate->field_224);
	SetAttribute(element, "gravity", mTemplate->field_40);
	SetAttribute(element, "gravity2", mTemplate->field_136);
	SetAttribute(element, "height", mTemplate->field_92);
	SetAttribute(element, "interval", mTemplate->field_68);
	SetAttribute(element, "intervalfxfile", mIntervalFxFile.c_str());//TODO: Preprocess and remove extension
	SetRangedQuadAttribute(element, "length", mTemplate->field_186);
	SetAttribute(element, "life", mTemplate->field_24);
	SetAttribute(element, "midcolor1", mTemplate->field_104);
	SetAttribute(element, "midcolor2", mTemplate->field_116);
	SetAttribute(element, "modelname", mTemplate->field_688.c_str());//TODO: Preprocess and remove extension
	SetAttribute(element, "name", mName.c_str());
	SetAttribute(element, "numsegments", mTemplate->field_218);
	SetRangedQuadAttribute(element, "offset", mTemplate->field_188);
	SetRangedVectorAttribute(element, "orientaxis", mTemplate->field_196);
	SetRangedVectorAttribute(element, "origin", mTemplate->field_176);
	SetRangedVectorAttribute(element, "origin2", mTemplate->field_208);
	
	if (mUsePlatformOverrides)
	{
		SetAttribute(element, "plat", mPlatformFlags);
	}

	SetAttribute(element, "plife", mTemplate->field_144);
	SetAttribute(element, "primitiveflags", mTemplate->field_48);
	SetAttribute(element, "primitiveflags2", mTemplate->field_52);
	SetAttribute(element, "pspawn", mTemplate->field_156);
	SetAttribute(element, "ptravel", mTemplate->field_148);
	SetAttribute(element, "radius", mTemplate->field_76);
	SetAttribute(element, "radius2", mTemplate->field_84);
	SetRangedQuadAttribute(element, "rotation", mTemplate->field_190);
	SetRangedVectorAttribute(element, "rotationaxis", mTemplate->field_194);
	SetRangedQuadAttribute(element, "rotationradius", mTemplate->field_192);
	SetAttribute(element, "shader", mTemplate->field_616.c_str());//TODO: Preprocess and remove extension
	SetAttribute(element, "shaderflags", mTemplate->field_684);
	SetTextureAttribute(element, "shadertexture0", mTemplate->field_226[0]);
	SetTextureAttribute(element, "shadertexture1", mTemplate->field_226[1]);
	SetTextureAttribute(element, "shadertexture2", mTemplate->field_226[2]);
	SetTextureAttribute(element, "shadertexture3", mTemplate->field_226[3]);
	SetTextureAttribute(element, "shadertexture4", mTemplate->field_226[4]);
	SetTextureAttribute(element, "shadertexture5", mTemplate->field_226[5]);
	SetAttribute(element, "shakescale", mTemplate->field_168);
	SetAttribute(element, "shakespeed", mTemplate->field_164);
	SetAttribute(element, "shaketype", mTemplate->field_172);
	SetRangedQuadAttribute(element, "size", mTemplate->field_182);
	SetRangedQuadAttribute(element, "size2", mTemplate->field_206);
	SetAttribute(element, "spawnflags", mTemplate->field_56);
	SetRangedQuadAttribute(element, "startarc", mTemplate->field_202);
	SetAttribute(element, "startcolor1", mTemplate->field_100);
	SetAttribute(element, "startcolor2", mTemplate->field_112);
	SetRangedVectorAttribute(element, "startvec", mTemplate->field_222);
	SetTextureAttribute(element, "texture", mTemplate->field_688);
	SetRangedVectorAttribute(element, "transformrotation", mTemplate->field_220);
	SetRangedQuadAttribute(element, "uvscale", mTemplate->field_216);
	SetRangedQuadAttribute(element, "uvscroll", mTemplate->field_214);
	SetRangedVectorAttribute(element, "velocity", mTemplate->field_178);
	SetRangedVectorAttribute(element, "velocity2", mTemplate->field_210);
	SetAttribute(element, "viewoffset", mTemplate->field_72);
}

void CFxSegment::SetEnabled(bool enabled)
{
	mEnabled = enabled;
	
	if (mTemplate)
	{
		mTemplate->field_0 = NULL;
		TheFxEditor().GetFile().UpdateTemplateLinks();
	}
}

void CFxSegment::SetPrimitiveFlag(unsigned int flag, bool set)
{
	assert(mTemplate);

	if (set)
	{
		mTemplate->field_48 |= flag;
	}
	else
	{
		mTemplate->field_48 &= ~flag;
	}
}

void CFxSegment::SetPlatformSupported(EFxPlatform platform, bool supported)
{
	if (!mUsePlatformOverrides)
	{
		return;
	}

	assert(platform != PLATFORM_ALL);
	
	unsigned int mask = 1 << platform;
	
	if (supported)
	{
		mPlatformFlags |= mask;
	}
	else
	{
		mPlatformFlags &= ~mask;
	}

	TheFxEditor().GetFile().UpdateTemplateLinks();//TODO: Do this only if it's not default filter?
}

bool CFxSegment::IsPlatformSupported(EFxPlatform platform) const
{
	if (platform == PLATFORM_ALL || !mUsePlatformOverrides)
	{
		return true;
	}

	return (mPlatformFlags & (1 << platform)) != 0;
}

void CFxSegment::SetFxLevelSupported(EFxLevel level, bool supported)
{
	assert(level >= LEVEL_1 && level <= LEVEL_8);
	assert(mTemplate);

	unsigned int mask = 0x1000000 << (level - 1);

	if (supported)
	{
		mTemplate->field_48 &= ~mask;
	}
	else
	{
		mTemplate->field_48 |= mask;
	}

	TheFxEditor().GetFile().UpdateTemplateLinks();//TODO: Do this only if it's not default filter?
}

unsigned int FxLevelMask(EFxLevel level)
{
	if (level >= LEVEL_1 && level <= LEVEL_8)
	{
		return 0x1000000 << (level - 1);
	}
	else
	{
		return 0;
	}
}

bool CFxSegment::IsFxLevelSupported(EFxLevel level) const
{
	assert(mTemplate);
	return (mTemplate->field_48 & FxLevelMask(level)) == 0;
}
