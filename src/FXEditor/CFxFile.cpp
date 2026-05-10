#include "CFxFile.h"

#include <FX/CFxTemplate.h>
#include <FX/CFxTemplateManager.h>

#include <FX/CFxPrimitiveTemplate.h>
#include <FX/CFxPrimitiveTemplatePool.h>

#include <FX/CRangedComponentPool.h>

#include <Misc/Common.h>
#include <Misc/ResourceSpecifier.h>
#include <Misc/IMemory.h>
#include <Misc/tinyXmlBinary.h>

#include <filesystem>
#include <tinyxml2/tinyxml2.h>

#include "IFxEditor.h"
#include "CFxFileParser.h"
#include "FxTemplateHelpers.h"

#include "Segments/CFxSegment.h"
#include "Segments/CFxEmptySegment.h"
#include "Segments/CFxParticleCloudSegment.h"
#include "Segments/CFxSpriteSegment.h"
#include "Segments/CFxOrientedSpriteSegment.h"
#include "Segments/CFxSparkSegment.h"
#include "Segments/CFxLineSegment.h"
#include "Segments/CFxCylinderSegment.h"
#include "Segments/CFxModelSegment.h"
#include "Segments/CFxLightSegment.h"
#include "Segments/CFxScreenFlashSegment.h"
#include "Segments/CFxCameraShakeSegment.h"
#include "Segments/CFxTrailSegment.h"

#include "../CXMLBFile.h"

namespace fs = std::filesystem;

const char* FILE_DEFAULT_NAME = "Untitled";

void CFxFile::UpdateTemplateLinks(bool skipCheck)
{
	assert(IsValid());

	mFileInfo.SetFirstPrimitiveTemplate(NULL);

	CFxPrimitiveTemplate* prevTemplate = NULL;

	for (IFxSegment* segment : mSegments)
	{
		CFxPrimitiveTemplate* curTemplate = segment->GetTemplate();
		
		if (skipCheck || (segment->GetEnabled() && TheFxEditor().GetFilter().Matches(segment)))
		{
			if (prevTemplate)
			{
				prevTemplate->field_0 = curTemplate;
			}
			else
			{
				mFileInfo.SetFirstPrimitiveTemplate(curTemplate);
			}

			prevTemplate = curTemplate;
		}
	}

	if (prevTemplate)
	{
		prevTemplate->field_0 = NULL;
	}
}

CFxFile::CFxFile()
{
	Clear();
}

void CFxFile::Clear(bool free)
{
	if (free && IsValid())
	{
		UpdateTemplateLinks(true);
	}

	for (IFxSegment* segment : mSegments)
	{
		delete segment;
	}

	mSegments.clear();

	if (free && IsValid())
	{
		mFileInfo.Free();
	}

	mName = FILE_DEFAULT_NAME;
	mDirty = false;
	mNextSegmentId = 0;
}

IFxSegment* CFxFile::AllocateFxSegment(EFXPrimType type)
{
	switch (type)
	{
	case FXPRIM_TYPE_EMPTY:
		return new CFxEmptySegment();
	case FXPRIM_TYPE_PARTICLE_CLOUD:
	case FXPRIM_TYPE_WEATHERFX:
		return new CFxParticleCloudSegment();
	case FXPRIM_TYPE_SPRITE:
		return new CFxSpriteSegment();
	case FXPRIM_TYPE_ORIENTED_SPRITE:
		return new CFxOrientedSpriteSegment();
	case FXPRIM_TYPE_SPARK:
		return new CFxSparkSegment();
	case FXPRIM_TYPE_LINE:
	case FXPRIM_TYPE_LIGHTNING:
	case FXPRIM_TYPE_BEZIER:
		return new CFxLineSegment();
	case FXPRIM_TYPE_CYLINDER:
		return new CFxCylinderSegment();
	case FXPRIM_TYPE_MODEL:
		return new CFxModelSegment();
	case FXPRIM_TYPE_LIGHT:
		return new CFxLightSegment();
	case FXPRIM_TYPE_SCREENFLASH:
		return new CFxScreenFlashSegment();
	case FXPRIM_TYPE_CAMERA_SHAKE:
		return new CFxCameraShakeSegment();
	case FXPRIM_TYPE_TRAIL:
		return new CFxTrailSegment();
	default:
		return new CFxSegment();
	}
}

void CFxFile::LinkSegment(IFxSegment* segment, CFxPrimitiveTemplate* primitive, bool freeRangedComponents)
{
	assert(segment);
	assert(primitive);

	CloneRangedComponents(primitive, freeRangedComponents);

	segment->SetId(mNextSegmentId++);
	segment->Initialize(primitive);
	mSegments.push_back(segment);
}

bool CFxFile::Create()
{
	SFxFileInfo fileInfo;

	if (!fileInfo.Allocate())
	{
		return false;
	}

	Clear();
	mFileInfo = fileInfo;
	return true;
}

bool CFxFile::LoadFromFile(const char* fileName)
{	
	CFxFileParser parser;
	SFxFileInfo fileInfo;

	if (!parser.Load(fileName, MP_MENU, fileInfo))
	{
		return false;
	}

	Clear();
	mFileInfo = fileInfo;

	for (auto& segmentTemplate : parser.GetSegmentTemplates())
	{
		LinkSegment(segmentTemplate.mSegment, segmentTemplate.mTemplate);
	}

	UpdateTemplateLinks();
	return true;
}

bool CFxFile::SaveToFile(const char* fileName)
{
	using namespace tinyxml2;

	assert(IsValid());
	
	XMLDocument doc;
	XMLElement* effect = doc.NewElement("Effect");

	if (!effect)
	{
		return false;
	}

	doc.InsertEndChild(effect);

	ratl::string_vs<128> str;

	snprintf(str.c_str(), str.capacity(), "%.4g", mFileInfo.mTemplate->field_4);
	effect->SetAttribute("looptime", str.c_str());

	snprintf(str.c_str(), str.capacity(), "%.4g", mFileInfo.mTemplate->field_8);
	effect->SetAttribute("randlooptime", str.c_str());

	effect->SetAttribute("persistloop", mFileInfo.mTemplate->field_12 ? 1 : 0);

	for (IFxSegment* segment : mSegments)
	{
		EFXPrimType primType = segment->GetTemplate()->field_4;

		if (primType != FXPRIM_TYPE_NONE && primType != FXPRIM_TYPE_TEXT)
		{
			const char* name = GetPrimitiveName(primType);
			XMLElement* primitive = effect->InsertNewChildElement(name);

			if (primitive)
			{
				segment->SetAttributes(primitive);
			}
		}
	}

	CXMLBFile xmlbFile;

	if (!xmlbFile.LoadFromXML(doc))
	{
		return false;
	}

	//fs::path filePath = fileName;
	//std::string cacheKey = filePath.generic_string();

	//filePath.replace_extension(".xml");

	//std::string rawFilePath = filePath.generic_string();

	//bool saved = doc.SaveFile(rawFilePath.c_str()) == XML_SUCCESS;

	bool saved = xmlbFile.WriteFile(fileName);

	if (saved)
	{
		mDirty = false;
		mFileInfo.UpdateCacheKey(fileName);
	}

	return saved;
}

bool CFxFile::CanAddSegment() const
{
	return IsValid() && CanAllocFxPrimitiveTemplate();
}

IFxSegment* CFxFile::NewSegment(EFXPrimType type)
{
	assert(IsValid());

	if (!CanAllocFxPrimitiveTemplate())
	{
		return NULL;
	}

	CFxPrimitiveTemplate* primitive = mFileInfo.AllocatePrimitiveTemplate();

	if (!primitive)
	{
		return NULL;
	}

	primitive->field_4 = type;

	IFxSegment* segment = AllocateFxSegment(type);

	if (!segment)
	{
		mFileInfo.FreePrimitiveTemplate(primitive);
		return NULL;
	}

	LinkSegment(segment, primitive);

	if (mFileInfo.IsPersistLoop())
	{
		segment->SetPrimitiveFlag(0x2, true);
	}

	MarkDirty();
	return segment;
}

void CFxFile::DeleteSegment(IFxSegment* segment)
{
	assert(IsValid());

	CFxPrimitiveTemplate* primTemplate = segment->GetTemplate();

	std::erase(mSegments, segment);
	delete segment;
	FreeFxPrimitiveTemplate(primTemplate);

	UpdateTemplateLinks();
	MarkDirty();
}

IFxSegment* CFxFile::CloneSegment(IFxSegment* segment)
{
	assert(IsValid());
	assert(segment);
	
	if (!CanAllocFxPrimitiveTemplate())
	{
		return NULL;
	}

	CFxPrimitiveTemplate* primTemplate = mFileInfo.AllocatePrimitiveTemplate();

	if (!primTemplate)
	{
		return NULL;
	}
	
	//Copy all properties
	*primTemplate = *segment->GetTemplate();
	//Set next primitive template to NULL
	primTemplate->field_0 = NULL;

	IFxSegment* clonedSegment = AllocateFxSegment(primTemplate->field_4);

	if (!clonedSegment)
	{
		mFileInfo.FreePrimitiveTemplate(primTemplate);
		return NULL;
	}

	clonedSegment->Clone(segment);
	LinkSegment(clonedSegment, primTemplate, false);
	//TODO: UpdateTemplateLinks();?
	MarkDirty();
	return clonedSegment;
}
