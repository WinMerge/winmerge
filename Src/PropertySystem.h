#pragma once

#include "UnicodeString.h"
#include <vector>
#include <memory>
#include <wtypes.h>

class PropertySystem
{
public:
	explicit PropertySystem(const std::vector<String>& canonicalNames);
	bool GetFormattedValues(const String& path, std::vector<String>& values);
	bool GetDisplayNames(std::vector<String>& names);
	const std::vector<String>& GetCanonicalNames() const { return m_canonicalNames; }
private:
	std::vector<String> m_canonicalNames;
#ifdef _WIN64
	std::vector<PROPERTYKEY> m_keys;
#endif
};