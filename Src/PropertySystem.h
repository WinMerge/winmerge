/** 
 * @file  PropertySystem.h
 *
 * @brief Declaration file for PropertySystem
 */
#pragma once

#include "UnicodeString.h"
#include <vector>
#include <memory>
#include <wtypes.h>

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
	bool GetFormattedValues(const String& path, std::vector<String>& values);
	bool GetDisplayNames(std::vector<String>& names);
	const std::vector<String>& GetCanonicalNames() const { return m_canonicalNames; }
private:
	void AddProperties(const std::vector<String>& canonicalNames);
	std::vector<String> m_canonicalNames;
	std::vector<PROPERTYKEY> m_keys;
};