#include "CXMLBFile.h"

#include <assert.h>
#include <algorithm>

#include <Misc/CMemory.h>

#include <igGapCore.h>
#include <igCore/igFile.h>

using namespace tinyxml2;

struct SXMLBHeader
{
	unsigned int mMagicNumber;
	unsigned int mVersion;
};

#define HEADER_SIZE (sizeof(SXMLBHeader))
#define ATTR_SIZE (sizeof(TiXmlAttribute))
#define ELEMENT_SIZE (sizeof(TiXmlElement) - ATTR_SIZE)

void ToLowerCase(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

bool CXMLBCache::CacheString(const char* str, bool lowerCase)
{
	std::string key(str);

	if (lowerCase)
	{
		ToLowerCase(key);
	}

	if (mStringLookup.find(key) == mStringLookup.end())
	{
		mStringLookup.insert(key);
		mCachedStrings.push_back(key);
		mStringBlockSize += key.length() + 1;
		return true;
	}

	return false;
}

bool CompareXMLAttributes(const XMLAttribute* a, const XMLAttribute* b)
{
	return stricmp(a->Name(), b->Name()) < 0;
}

void CXMLBCache::CacheElement(const XMLElement* element)
{
	CacheString(element->Name());

	std::vector<const XMLAttribute*> attrs;

	for (const XMLAttribute* attr = element->FirstAttribute();
		attr;
		attr = attr->Next())
	{
		attrs.push_back(attr);
	}

	std::sort(attrs.begin(), attrs.end(), CompareXMLAttributes);

	for (const XMLAttribute* attr : attrs)
	{
		CacheString(attr->Name(), true);
		CacheString(attr->Value());
	}

	mElementBlockSize += ELEMENT_SIZE + ATTR_SIZE * attrs.size();

	for (const XMLElement* child = element->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		CacheElement(child);
	}
}

CXMLBCache::CXMLBCache() :
	mElementBlockSize(0),
	mStringBlockSize(0)
{
}

void CXMLBCache::Clear()
{
	mElementBlockSize = 0;
	mStringBlockSize = 0;
	mCachedStrings.clear();
	mStringLookup.clear();
}

size_t CXMLBCache::Initialize(const XMLElement* root)
{
	assert(root);

	Clear();
	CacheElement(root);
	return HEADER_SIZE + mElementBlockSize + mStringBlockSize;
}

void CXMLBCache::InitStringBlock(char* buffer, TStringSet& lookup)
{
	lookup.clear();

	char* stringBlock = &buffer[HEADER_SIZE + mElementBlockSize];

	for (const std::string& str : mCachedStrings)
	{
		str.copy(stringBlock, str.size());
		stringBlock[str.size()] = '\0';

		std::string_view key(stringBlock, str.size());

		lookup.insert(key);
		stringBlock += str.size() + 1;
	}

	mCachedStrings.clear();
	mStringLookup.clear();
}

void SortAttributes(TiXmlElement* element)
{
	TiXmlAttribute* begin = &element->mAttributes[0];
	TiXmlAttribute* end = &element->mAttributes[element->mNumAttributes];
	
	std::sort(begin, end, [](const TiXmlAttribute& a, const TiXmlAttribute& b)
	{
		return stricmp(a.Name().c_str(), b.Name().c_str()) < 0;
	});
}

TiXmlElement* CXMLBFile::AllocElement(const XMLElement* element, void*& memory)
{
	TiXmlElement* xmlbElement = new (memory) TiXmlElement();

	xmlbElement->mName = LookupString(element->Name());
	xmlbElement->mNext_sibling = NULL;
	xmlbElement->mChild = NULL;

	int attrCount = 0;

	for (const XMLAttribute* attr = element->FirstAttribute();
		attr;
		attr = attr->Next())
	{
		TiXmlAttribute& xmlbAttr = xmlbElement->mAttributes[attrCount++];

		std::string name(attr->Name());
		ToLowerCase(name);

		xmlbAttr.mName = LookupString(name.c_str());
		xmlbAttr.mValue = LookupString(attr->Value());
	}

	xmlbElement->mNumAttributes = attrCount;

	SortAttributes(xmlbElement);

	memory = (char*)memory + ELEMENT_SIZE + ATTR_SIZE * attrCount;

	TiXmlElement* prev = NULL;

	for (const XMLElement* child = element->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		TiXmlElement* allocated = AllocElement(child, memory);

		if (!xmlbElement->mChild) xmlbElement->mChild = allocated;
		if (prev) prev->mNext_sibling = allocated;
		prev = allocated;
	}

	return xmlbElement;
}

TiXmlString* CXMLBFile::LookupString(const char* str)
{
	std::string_view key(str);
	auto it = mStringLookup.find(key);

	if (it != mStringLookup.end())
	{
		return (TiXmlString*)it->data();
	}

	return NULL;
}

void CXMLBFile::ConvertPointersToOffsets()
{
	assert(mBuffer);
	
	uintptr_t* cur = (uintptr_t*)&mBuffer[HEADER_SIZE];
	uintptr_t* end = (uintptr_t*)*cur;

	if (mSize > HEADER_SIZE)
	{
		int ptrCount = 3;

		while (cur < end)
		{
			if (ptrCount)
			{
				assert((char*)cur > mBuffer && (char*)cur < mBuffer + mSize);
				
				char* value = (char*)*cur;

				int offset = -1;

				if (value)
				{
					offset = value - mBuffer;
				}

				*cur = offset;
				ptrCount--;
			}
			else
			{
				int attrCount = *cur;

				assert(attrCount >= 0 && attrCount < 1000);
				ptrCount = 3 + 2 * attrCount;
			}

			cur++;
		}
	}
}

CXMLBFile::CXMLBFile() : 
	mBuffer(NULL), 
	mSize(0)
{
}

CXMLBFile::~CXMLBFile()
{
	Clear();
}

void CXMLBFile::Clear()
{
	if (mBuffer)
	{
		xfree(mBuffer, MP_16);
	}

	mBuffer = NULL;
	mSize = 0;
}

bool CXMLBFile::LoadFromXML(const XMLDocument& doc)
{
	Clear();

	if (doc.Error())
	{
		return false;
	}

	const XMLElement* root = doc.RootElement();

	if (!root)
	{
		return false;
	}

	CXMLBCache cache;
	size_t size = cache.Initialize(root);

	char* buffer = (char*)xmalloc(size, MP_16, 51);

	if (!buffer)
	{
		return false;
	}
	
	mBuffer = buffer;
	mSize = size;

	SXMLBHeader& header = *(SXMLBHeader*)&buffer[0];

	header.mMagicNumber = 0x11B1;
	header.mVersion = 1;

	if (size > 8)
	{
		cache.InitStringBlock(buffer, mStringLookup);

		void* firstElement = &buffer[HEADER_SIZE];

		if (AllocElement(root, firstElement))
		{
			ConvertPointersToOffsets();
		}
	}

	return true;
}

bool CXMLBFile::WriteFile(const char* fileName)
{
	if (!mBuffer)
	{
		return false;
	}

	bool success = true;

	igFileRef file = NewInstanceRef<igFile>(MP_16);

	if (file->fopen(fileName, "wb"))
	{
		if (file->fwrite(mBuffer, mSize, 1) != 1)
		{
			success = false;
		}

		file->fclose();
	}

	return success;
}
