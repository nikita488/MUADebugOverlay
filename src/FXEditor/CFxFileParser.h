#pragma once

#include <vector>
#include <string>

#include <Misc/IMemory.h>

class CFxFile;
class SFxFileInfo;

class TiXmlElement;
class TiXmlAttribute;

class CFxTemplate;
class CFxPrimitiveTemplate;
class IFxSegment;

struct SFxSegmentTemplate
{
	IFxSegment* mSegment;
	CFxPrimitiveTemplate* mTemplate;
};

typedef std::vector<SFxSegmentTemplate> TFxSegmentTemplateVector;

class CFxFileParser
{
	friend class CFxFile;

private:
	TFxSegmentTemplateVector mSegmentTemplates;

private:
	bool ParseFxPrimitiveAttribute(TiXmlAttribute* attr, SFxSegmentTemplate& segmentTemplate);
	bool ParseFxPrimitive(TiXmlElement* element, CFxTemplate* fxTemplate);

	bool ParseFxAttribute(TiXmlAttribute* attr, CFxTemplate* fxTemplate);
	bool ParseFx(const char* filePath, int& handle);

public:
	bool Load(const char* fileName, EMemoryPoolID poolId, SFxFileInfo& fileInfo);

	inline const TFxSegmentTemplateVector& GetSegmentTemplates() const
	{
		return mSegmentTemplates;
	}
};
