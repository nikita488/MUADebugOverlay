#pragma once

#include <string>
#include <string_view>

#include <vector>
#include <unordered_set>

#include <tinyxml2/tinyxml2.h>
#include <Misc/tinyXmlBinary.h>

typedef std::unordered_set<std::string_view> TStringSet;

class CXMLBCache
{
private:
	size_t mElementBlockSize;
	size_t mStringBlockSize;
	std::vector<std::string> mCachedStrings;
	std::unordered_set<std::string> mStringLookup;

private:
	bool CacheString(const char* str, bool lowerCase = false);

	void CacheElement(const tinyxml2::XMLElement* element);

public:
	CXMLBCache();

	void Clear();

	size_t Initialize(const tinyxml2::XMLElement* root);

	void InitStringBlock(char* buffer, TStringSet& lookup);
};

class CXMLBFile
{
private:
	char* mBuffer;
	size_t mSize;
	TStringSet mStringLookup;

private:
	TiXmlElement* AllocElement(const tinyxml2::XMLElement* element, void*& memory);
	TiXmlString* LookupString(const char* str);
	void ConvertPointersToOffsets();

public:
	CXMLBFile();

	~CXMLBFile();

	void Clear();

	bool LoadFromXML(const tinyxml2::XMLDocument& doc);

	bool WriteFile(const char* fileName);
};
