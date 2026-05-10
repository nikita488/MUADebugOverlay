#include "FxTemplateHelpers.h"

#include <Misc/ResourceSpecifier.h>

#include <FX/CFxTemplate.h>
#include <FX/CFxPrimitiveTemplate.h>

#include <FX/CFxTemplateManager.h>
#include <FX/CFxPrimitiveTemplatePool.h>
#include <FX/CRangedComponentPool.h>

const char* fxPrimitiveNames[FXPRIM_TYPE_NUM_TYPES] =
{
	"NULL",
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
	"Text",
};

const char* fxPrimitiveDisplayNames[FXPRIM_TYPE_NUM_TYPES] =
{
	"NULL",
	"Empty",
	"Particle Cloud",
	"Sprite",
	"Weather Effect",
	"Oriented Sprite",
	"Spark",
	"Line",
	"Lightning",
	"Cylinder",
	"Model",
	"Light",
	"Screen Flash",
	"Camera Shake",
	"Trail",
	"Bezier Curve",
	"Text",
};

const char* GetPrimitiveName(EFXPrimType primType)
{
	return fxPrimitiveNames[primType];
}

const char* GetPrimitiveDisplayName(EFXPrimType primType)
{
	return fxPrimitiveDisplayNames[primType];
}

bool CanAllocFxTemplate()
{
	CFxTemplateManager& manager = TheFxTemplateManager();
	return !manager.field_19416.full();
}

bool CanAllocFxPrimitiveTemplate()
{
	CFxPrimitiveTemplatePool& primitivePool = TheFxPrimitiveTemplatePool();
	TVectorPool& vectorPool = TheVectorPool();
	TQuadPool& quadPool = TheQuadPool();

	int freePrimitiveCount = primitivePool.field_0.capacity() - primitivePool.field_0.size();
	int freeVectorCount = vectorPool.field_0.capacity() - vectorPool.field_0.size();
	int freeQuadCount = quadPool.field_0.capacity() - quadPool.field_0.size();

	return freePrimitiveCount >= 1 && freeVectorCount >= 11 && freeQuadCount >= 13;
}

bool FreeFxTemplate(CFxTemplate* fxTemplate, EMemoryPoolID poolId)
{
	assert(fxTemplate);

	if (fxTemplate->field_16 == poolId)
	{
		fxTemplate->Free();
		TheFxTemplateManager().field_19416.free(fxTemplate);
		return true;
	}

	return false;
}

bool FreeFxTemplate(const char* resSpec)
{
	assert(resSpec && resSpec[0]);

	ratl::string_vs<256> resSpecBuf(resSpec);
	EMemoryPoolID poolId;

	if (!ResourceSpecifier_PoolID(resSpecBuf, poolId))
	{
		return false;
	}

	CFxTemplateManager& manager = TheFxTemplateManager();
	CFxTemplateManager::SFxFileRecord record(resSpec);

	auto& it = manager.field_0.find(record);

	if (it != manager.field_0.end())
	{
		int handle = it.value();

		if (!handle)
		{
			return false;
		}

		CFxTemplate& fxTemplate = manager.field_19416[handle];

		if (FreeFxTemplate(&fxTemplate, poolId))
		{
			manager.field_0.erase(it);
			return true;
		}
	}

	return false;
}

void FreeFxPrimitiveTemplate(CFxPrimitiveTemplate* fxPrimitive)
{
	assert(fxPrimitive);
	TheFxPrimitiveTemplatePool().field_0.free(fxPrimitive);
}

int CloneVector(int handle)
{
	return TheVectorPool().field_0.alloc(TheVectorPool().Get(handle));
}

int CloneQuad(int handle)
{
	return TheQuadPool().field_0.alloc(TheQuadPool().Get(handle));
}

void CloneRangedComponents(CFxPrimitiveTemplate* primitive, bool free)
{
	int field_176 = CloneVector(primitive->field_176);
	int field_178 = CloneVector(primitive->field_178);
	int field_180 = CloneVector(primitive->field_180);
	int field_182 = CloneQuad(primitive->field_182);
	int field_184 = CloneQuad(primitive->field_184);
	int field_186 = CloneQuad(primitive->field_186);
	int field_188 = CloneQuad(primitive->field_188);
	int field_190 = CloneQuad(primitive->field_190);
	int field_192 = CloneQuad(primitive->field_192);
	int field_194 = CloneVector(primitive->field_194);
	int field_196 = CloneVector(primitive->field_196);
	int field_198 = CloneQuad(primitive->field_198);
	int field_200 = CloneQuad(primitive->field_200);
	int field_202 = CloneQuad(primitive->field_202);
	int field_204 = CloneQuad(primitive->field_204);
	int field_206 = CloneQuad(primitive->field_206);
	int field_208 = CloneVector(primitive->field_208);
	int field_210 = CloneVector(primitive->field_210);
	int field_212 = CloneVector(primitive->field_212);
	int field_214 = CloneQuad(primitive->field_214);
	int field_216 = CloneQuad(primitive->field_216);
	int field_220 = CloneVector(primitive->field_220);
	int field_222 = CloneVector(primitive->field_222);
	int field_224 = CloneVector(primitive->field_224);

	if (free)
	{
		CFxPrimitiveTemplate* nextTemplate = primitive->field_0;

		primitive->Free();
		primitive->field_0 = nextTemplate;
	}

	primitive->field_176 = field_176;
	primitive->field_178 = field_178;
	primitive->field_180 = field_180;
	primitive->field_182 = field_182;
	primitive->field_184 = field_184;
	primitive->field_186 = field_186;
	primitive->field_188 = field_188;
	primitive->field_190 = field_190;
	primitive->field_192 = field_192;
	primitive->field_194 = field_194;
	primitive->field_196 = field_196;
	primitive->field_198 = field_198;
	primitive->field_200 = field_200;
	primitive->field_202 = field_202;
	primitive->field_204 = field_204;
	primitive->field_206 = field_206;
	primitive->field_208 = field_208;
	primitive->field_210 = field_210;
	primitive->field_212 = field_212;
	primitive->field_214 = field_214;
	primitive->field_216 = field_216;
	primitive->field_220 = field_220;
	primitive->field_222 = field_222;
	primitive->field_224 = field_224;
}


