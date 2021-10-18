/** 
 * @file  PropertySystem.h
 *
 * @brief Declaration file for PropertySystem
 */
#pragma once

#include "UnicodeString.h"
#include <vector>
#include <memory>
#include <combaseapi.h>

class PropertyValues
{
public:
	PropertyValues();
	~PropertyValues();
	static int CompareValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index);
	static int64_t DiffValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index, bool& numeric);
	static int CompareAllValues(const PropertyValues& values1, const PropertyValues& values2);
	std::vector<PROPVARIANT> m_values;
};

class PropertySystem
{
public:
	enum ENUMFILTER {
		ALL,
		SYSTEM,
		NONSYSTEM,
		VIEWABLE,
		QUERYABLE,
		INFULLTEXTQUERY,
		COLUMN
	};
	explicit PropertySystem(ENUMFILTER filter);
	explicit PropertySystem(const std::vector<String>& canonicalNames);
	bool GetPropertyValues(const String& path, PropertyValues& values);
	String FormatPropertyValue(const PropertyValues& values, unsigned index);
	bool GetDisplayNames(std::vector<String>& names);
	const std::vector<String>& GetCanonicalNames() const { return m_canonicalNames; }
private:
	void AddProperties(const std::vector<String>& canonicalNames);
	std::vector<String> m_canonicalNames;
	std::vector<PROPERTYKEY> m_keys;
};