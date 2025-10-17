/** 
 * @file  PropertySystem.h
 *
 * @brief Declaration file for PropertySystem
 */
#pragma once

#include "UnicodeString.h"
#include <vector>
#include <memory>
#if __has_include(<PropIdl.h>)
#include <PropIdl.h>
#else
typedef int PROPVARIANT;
typedef int PROPERTYKEY;
#endif

class PropertyValues
{
	friend class PropertySystem;
public:
	PropertyValues();
	~PropertyValues();
	static int CompareValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index);
	static int64_t DiffValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index, bool& numeric);
	static int CompareAllValues(const PropertyValues& values1, const PropertyValues& values2);
	bool IsEmptyValue(size_t index) const;
	bool IsHashValue(size_t index) const;
	std::vector<uint8_t> GetHashValue(size_t index) const;
	size_t GetSize() const { return m_values.size(); }
	void Resize(size_t size) { m_values.resize(size); }
	PROPVARIANT& operator[](size_t index) { return m_values[index]; }
private:
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
	int GetPropertyIndex(const String& canonicalName);
	bool GetPropertyType(unsigned index, VARTYPE& vt) const;
	String FormatPropertyValue(const PropertyValues& values, unsigned index);
	bool GetDisplayNames(std::vector<String>& names);
	bool HasHashProperties() const;
	const std::vector<String>& GetCanonicalNames() const { return m_canonicalNames; }
private:
	bool AddProperty(const String& canonicalName);
	void AddProperties(const std::vector<String>& canonicalNames);
	std::vector<String> m_canonicalNames;
	std::vector<PROPERTYKEY> m_keys;
	bool m_onlyHashProperties = true;
};