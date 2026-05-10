#include "CFxFileParser.h"

#include <FX/CFxTemplate.h>
#include <FX/CFxPrimitiveTemplate.h>
#include <FX/CFxTemplateManager.h>

#include <Misc/IMemory.h>
#include <Misc/Common.h>
#include <Misc/ResourceSpecifier.h>
#include <Misc/tinyXmlBinary.h>

#include "IFxEditor.h"
#include "FxTemplateHelpers.h"
#include "CFxFileInfo.h"
#include "Segments/IFxSegment.h"

bool CFxFileParser::ParseFxAttribute(TiXmlAttribute* attr, CFxTemplate* fxTemplate)
{
	assert(attr);

	const char* name = attr->Name().c_str();
	const char* value = attr->Value().c_str();

	if (!stricmp(name, "name"))
	{
		return true;
	}
	else if (!stricmp(name, "LoopTime"))
	{
		return fxTemplate->ParseFloat(fxTemplate->field_4, value);
	}
	else if (!stricmp(name, "RandLoopTime"))
	{
		return fxTemplate->ParseFloat(fxTemplate->field_8, value);
	}
	else if (!stricmp(name, "PersistLoop"))
	{
		int persistLoop;
		bool parsed = fxTemplate->ParseInteger(persistLoop, value);
		fxTemplate->field_12 = persistLoop != 0;
		return parsed;
	}
	else
	{
		return false;
	}
}

bool CFxFileParser::ParseFxPrimitiveAttribute(TiXmlAttribute* attr, SFxSegmentTemplate& segmentTemplate)
{
	IFxSegment* segment = segmentTemplate.mSegment;
	CFxPrimitiveTemplate* primTemplate = segmentTemplate.mTemplate;
	
	assert(attr);
	assert(segment);
	assert(primTemplate);
	
	const char* name = attr->Name().c_str();
	const char* value = attr->Value().c_str();

	if (!name || !value)
	{
		return false;
	}

	if (!stricmp(name, "name"))
	{
		segment->SetName(value);
	}
	else if (!stricmp(name, "plat"))
	{
		unsigned int platformFlags;
		bool parsed = primTemplate->ParseUnsignedInteger(platformFlags, value);

		segment->SetUsePlatformOverrides(true);
		segment->SetPlatformFlags(platformFlags);
		return parsed;
	}
	else if (!stricmp(name, "transformRotation"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_220, value);
	}
	else if (!stricmp(name, "pLife"))
	{
		return primTemplate->ParseFloat(primTemplate->field_144, value);
	}
	else if (!stricmp(name, "count"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_16, value);
	}
	else if (!stricmp(name, "life"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_24, value);
	}
	else if (!stricmp(name, "delay"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_8, value);
	}
	else if (!stricmp(name, "origin"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_176, value);
	}
	else if (!stricmp(name, "velocity"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_178, value);
	}
	else if (!stricmp(name, "acceleration"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_180, value);
	}
	else if (!stricmp(name, "gravity"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_40, value);
	}
	else if (!stricmp(name, "drag"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_32, value);
	}
	else if (!stricmp(name, "viewoffset"))
	{
		return primTemplate->ParseFloat(primTemplate->field_72, value);
	}
	else if (!stricmp(name, "origin2"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_208, value);
	}
	else if (!stricmp(name, "velocity2"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_210, value);
	}
	else if (!stricmp(name, "acceleration2"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_212, value);
	}
	else if (!stricmp(name, "gravity2"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_136, value);
	}
	else if (!stricmp(name, "drag2"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_128, value);
	}
	else if (!stricmp(name, "ptravel"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_148, value);
	}
	else if (!stricmp(name, "pspawn"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_156, value);
	}
	else if (!stricmp(name, "startVec"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_222, value);
	}
	else if (!stricmp(name, "endVec"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_224, value);
	}
	else if (!stricmp(name, "radius"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_76, value);
	}
	else if (!stricmp(name, "radius2"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_84, value);
	}
	else if (!stricmp(name, "height"))
	{
		return primTemplate->ParseRangedFloat(primTemplate->field_92, value);
	}
	else if (!stricmp(name, "startColor1"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_100, value);
	}
	else if (!stricmp(name, "midColor1"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_104, value);
	}
	else if (!stricmp(name, "endColor1"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_108, value);
	}
	else if (!stricmp(name, "startColor2"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_112, value);
	}
	else if (!stricmp(name, "midColor2"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_116, value);
	}
	else if (!stricmp(name, "endColor2"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_120, value);
	}
	else if (!stricmp(name, "alpha"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_184, value);
	}
	else if (!stricmp(name, "size"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_182, value);
	}
	else if (!stricmp(name, "size2"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_206, value);
	}
	else if (!stricmp(name, "length"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_186, value);
	}
	else if (!stricmp(name, "offset"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_188, value);
	}
	else if (!stricmp(name, "rotation"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_190, value);
	}
	else if (!stricmp(name, "rotationRadius"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_192, value);
	}
	else if (!stricmp(name, "rotationAxis"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_194, value);
	}
	else if (!stricmp(name, "orientAxis"))
	{
		return primTemplate->ParseRangedVector(primTemplate->field_196, value);
	}
	else if (!stricmp(name, "chaos"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_198, value);
	}
	else if (!stricmp(name, "attenuation"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_200, value);
	}
	else if (!stricmp(name, "startarc"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_202, value);
	}
	else if (!stricmp(name, "endarc"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_204, value);
	}
	else if (!stricmp(name, "primitiveFlags"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_48, value);
	}
	else if (!stricmp(name, "primitiveFlags2"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_52, value);
	}
	else if (!stricmp(name, "spawnFlags"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_56, value);
	}
	else if (!stricmp(name, "DeathFxFile"))
	{
		segment->SetDeathFxFile(value);
	}
	else if (!stricmp(name, "IntervalFxFile"))
	{
		segment->SetIntervalFxFile(value);
	}
	else if (!stricmp(name, "Interval"))
	{
		primTemplate->field_68 = float(atof(value));
	}
	else if (!stricmp(name, "texture"))
	{
		if (strlen(value))
		{
			primTemplate->field_688 = value;
		}
	}
	else if (!stricmp(name, "shadertexture0"))
	{
		if (strlen(value))
		{
			primTemplate->field_226[0] = value;
		}
	}
	else if (!stricmp(name, "shadertexture1"))
	{
		if (strlen(value))
		{
			primTemplate->field_226[1] = value;
		}
	}
	else if (!stricmp(name, "shadertexture2"))
	{
		if (strlen(value))
		{
			primTemplate->field_226[2] = value;
		}
	}
	else if (!stricmp(name, "shadertexture3"))
	{
		if (strlen(value))
		{
			primTemplate->field_226[3] = value;
		}
	}
	else if (!stricmp(name, "shadertexture4"))
	{
		if (strlen(value))
		{
			primTemplate->field_226[4] = value;
		}
	}
	else if (!stricmp(name, "shadertexture5"))
	{
		if (strlen(value))
		{
			primTemplate->field_226[5] = value;
		}
	}
	else if (!stricmp(name, "shader"))
	{
		if (strlen(value))
		{
			primTemplate->field_616 = value;
		}
	}
	else if (!stricmp(name, "shaderFlags"))
	{
		return primTemplate->ParseUnsignedInteger(primTemplate->field_684, value);
	}
	else if (!stricmp(name, "modelName"))
	{
		primTemplate->field_688 = value;
	}
	else if (!stricmp(name, "blend"))
	{
		return primTemplate->ParseBlend(value);
	}
	else if (!stricmp(name, "uvscroll"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_214, value);
	}
	else if (!stricmp(name, "uvscale"))
	{
		return primTemplate->ParseRangedQuadratic(primTemplate->field_216, value);
	}
	else if (!stricmp(name, "shaketype"))
	{
		return primTemplate->ParseInteger(primTemplate->field_172, value);
	}
	else if (!stricmp(name, "shakescale"))
	{
		return primTemplate->ParseFloat(primTemplate->field_168, value);
	}
	else if (!stricmp(name, "shakespeed"))
	{
		return primTemplate->ParseFloat(primTemplate->field_168, value);
	}
	else if (!stricmp(name, "numsegments"))
	{
		int numSegments;
		bool parsed = primTemplate->ParseInteger(numSegments, value);
		primTemplate->field_218 = numSegments;
		return parsed;
	}
	else
	{
		return false;
	}

	return true;
}

IDTable* EPrimTypeToStringTable = (IDTable*)0x805860;

bool CFxFileParser::ParseFxPrimitive(TiXmlElement* element, CFxTemplate* fxTemplate)
{
	assert(fxTemplate);

	CFxFile& file = TheFxEditor().GetFile();
	
	if (!CanAllocFxPrimitiveTemplate())
	{
		return false;
	}

	TiXmlAttribute* attr = element->GetAttribute(0);

	if (!attr)
	{
		return false;
	}

	EFXPrimType primType = FXPRIM_TYPE_NONE;
	StringToID(EPrimTypeToStringTable, element->Value().c_str(), (int*)&primType);

	if (primType == FXPRIM_TYPE_NONE)
	{
		return false;
	}

	CFxPrimitiveTemplate* primTemplate = fxTemplate->AllocFxPrimitiveTemplate();

	if (!primTemplate)
	{
		return false;
	}

	primTemplate->field_4 = primType;

	IFxSegment* segment = file.AllocateFxSegment(primType);

	if (!segment)
	{
		fxTemplate->FreeFxPrimitiveTemplate(primTemplate);
		return false;
	}

	SFxSegmentTemplate segmentTemplate;

	segmentTemplate.mSegment = segment;
	segmentTemplate.mTemplate = primTemplate;

	for (int i = 0; i < element->NumAttributes(); i++)
	{
		attr = element->GetAttribute(i);

		if (attr)
		{
			ParseFxPrimitiveAttribute(attr, segmentTemplate);
		}
	}

	mSegmentTemplates.push_back(segmentTemplate);
	return true;
}

bool CFxFileParser::ParseFx(const char* filePath, int& handle)
{
	if (!CanAllocFxTemplate())
	{
		return false;
	}
	
	TiXmlDocument doc;

	if (!doc.LoadFile(filePath, false, MP_TEMPORARY, false))
	{
		return false;
	}

	TiXmlElement* root = doc.FirstChild("Effect");

	if (!root)
	{
		return false;
	}

	TiXmlElement* primitive = root->IterateChildren(NULL);

	if (!primitive)
	{
		return false;
	}

	CFxTemplate* fxTemplate = TheFxTemplateManager().AllocFxTemplate(&handle);

	if (!handle || !fxTemplate)
	{
		return false;
	}
	
	for (int i = 0; i < root->NumAttributes(); i++)
	{
		TiXmlAttribute* attr = root->GetAttribute(i);

		if (attr)
		{
			ParseFxAttribute(attr, fxTemplate);
		}
	}

	do
	{
		ParseFxPrimitive(primitive, fxTemplate);
		primitive = root->IterateChildren(primitive);
	}
	while (primitive);

	return true;
}

int IsFileRegistered(CFxTemplateManager::SFxFileRecord& record, EMemoryPoolID poolId)
{
	CFxTemplateManager& manager = TheFxTemplateManager();
	CFxTemplateManager::TFxFileRecordMapIt it = manager.field_0.find(record);

	if (it != manager.field_0.end() &&
		it.value() &&
		manager.field_19416[it.value()].field_16 == poolId)
	{
		return it.value();
	}

	return 0;
}

bool CFxFileParser::Load(const char* fileName, EMemoryPoolID poolId, SFxFileInfo& fileInfo)
{
	if (!fileName || !fileName[0])
	{
		return false;
	}

	const char* fixedPath = Filename_EnsureForwardSlashes(fileName);

	ratl::string_vs<256> fullPath(Filename_FixFilename(fixedPath, "effects/", ".xmlb"));
	ratl::string_vs<256> resSpec;

	MakeResourceSpecifier(poolId, fullPath.c_str(), resSpec);

	CFxTemplateManager::SFxFileRecord record(resSpec.c_str());
	int handle = IsFileRegistered(record, poolId);

	if (!handle && ParseFx(fullPath.c_str(), handle))
	{
		TheFxTemplateManager().field_0.insert(record, handle);
	}

	if (!handle)
	{
		return false;
	}

	fileInfo.mTemplate = &TheFxTemplateManager().field_19416[handle];
	fileInfo.mTemplateHandle = handle;
	fileInfo.mCacheKey = resSpec.c_str();
	fileInfo.SetMemoryPool(poolId);
	return true;
}
