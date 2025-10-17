/** 
 * @file  PropertySystem.cpp
 *
 * @brief Implementation file for PropertySystem
 */
#include "pch.h"
#include "PropertySystem.h"
#include "HashCalc.h"
#ifdef _WIN64
#include <shobjidl.h>
#include <propsys.h>
#include <propvarutil.h>
#include <strsafe.h>
#endif

#ifdef _WIN64

#pragma comment(lib, "propsys.lib")
 
 // {ECA2D096-7C87-4DFF-94E7-E7FE9BA34BE8} 100-102
static const PROPERTYKEY PKEY_HASH_MD5 = { {0xeca2d096, 0x7c87, 0x4dff, 0x94, 0xe7, 0xe7, 0xfe, 0x9b, 0xa3, 0x4b, 0xe8}, 100 };
static const PROPERTYKEY PKEY_HASH_SHA1 ={ {0xeca2d096, 0x7c87, 0x4dff, 0x94, 0xe7, 0xe7, 0xfe, 0x9b, 0xa3, 0x4b, 0xe8}, 101 }; 
static const PROPERTYKEY PKEY_HASH_SHA256 = { {0xeca2d096, 0x7c87, 0x4dff, 0x94, 0xe7, 0xe7, 0xfe, 0x9b, 0xa3, 0x4b, 0xe8}, 102 };

struct PROPERTYINFO
{
	const PROPERTYKEY* pKey;
	const wchar_t *pszCanonicalName;
	const wchar_t *pszDisplayName;
};

static const PROPERTYINFO g_HashProperties[] =
{
	{ &PKEY_HASH_MD5,    L"Hash.MD5",    L"MD5" },
	{ &PKEY_HASH_SHA1,   L"Hash.SHA1",   L"SHA1" },
	{ &PKEY_HASH_SHA256, L"Hash.SHA256", L"SHA256" },
};

static int GetPropertyIndexFromKey(const PROPERTYKEY& key)
{
	for (size_t i = 0; i < std::size(g_HashProperties); ++i)
	{
		if (key == *g_HashProperties[i].pKey)
			return static_cast<int>(i);
	}
	return -1;
}

static const PROPERTYKEY *GetPropertyKeyFromName(const String& canonicalName)
{
	for (const auto& prop : g_HashProperties)
	{
		if (canonicalName == prop.pszCanonicalName)
			return prop.pKey;
	}
	return nullptr;
}

static bool CalculateHashValue(const String& path, const PROPERTYKEY& key, PROPVARIANT& value)
{
	int i = GetPropertyIndexFromKey(key);
	if (i >= 0)
	{
		std::vector<uint8_t> hash;
		HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CalculateHashValue(hFile, g_HashProperties[i].pszDisplayName, hash);
			CloseHandle(hFile);
		}
		InitPropVariantFromBuffer(hash.data(), static_cast<unsigned>(hash.size()), &value);
		return true;
	}
	return false;
}

PropertyValues::PropertyValues() = default;

PropertyValues::~PropertyValues()
{
	for (auto& value : m_values)
		PropVariantClear(&value);
}

int PropertyValues::CompareValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index)
{
	if (index >= values1.m_values.size())
		return -1;
	if (index >= values2.m_values.size())
		return 1;
	return PropVariantCompare(values1.m_values[index], values2.m_values[index]);
}

int64_t PropertyValues::DiffValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index, bool& numeric)
{
	numeric = false;
	if (index >= values1.m_values.size())
		return -1;
	if (index >= values2.m_values.size())
		return 1;
	short vt1 = values1.m_values[index].vt;
	short vt2 = values2.m_values[index].vt;
	if (vt1 == VT_I8 || vt1 == VT_UI8 || vt2 == VT_I8 || vt2 == VT_UI8)
	{
		numeric = true;
		return values2.m_values[index].cyVal.int64 - values1.m_values[index].cyVal.int64;
	}
	else if (vt1 == VT_I4 || vt1 == VT_UI4 || vt2 == VT_I4 || vt2 == VT_UI4)
	{
		numeric = true;
		return values2.m_values[index].intVal - values1.m_values[index].intVal;
	}
	return PropVariantCompare(values1.m_values[index], values2.m_values[index]);
}

int PropertyValues::CompareAllValues(const PropertyValues& values1, const PropertyValues& values2)
{
	if (values1.m_values.size() < values2.m_values.size())
		return -1;
	if (values1.m_values.size() > values2.m_values.size())
		return 1;
	for (size_t i = 0; i < values1.m_values.size(); ++i)
	{
		int result = PropVariantCompare(values1.m_values[i], values2.m_values[i]);
		if (result != 0)
			return result;
	}
	return 0;
}

bool PropertyValues::IsEmptyValue(size_t index) const
{
	return (index < m_values.size() && m_values[index].vt == VT_EMPTY);
}

bool PropertyValues::IsHashValue(size_t index) const
{
	return (index < m_values.size() && m_values[index].vt == (VT_VECTOR | VT_UI1));
}

std::vector<uint8_t> PropertyValues::GetHashValue(size_t index) const
{
	if (index >= m_values.size() || m_values[index].vt != (VT_VECTOR | VT_UI1))
		return {};
	return { m_values[index].caub.pElems, m_values[index].caub.pElems + m_values[index].caub.cElems };
}

PropertySystem::PropertySystem(ENUMFILTER filter)
{
	IPropertyDescriptionList* ppdl = nullptr;
	if (SUCCEEDED(PSEnumeratePropertyDescriptions(static_cast<PROPDESC_ENUMFILTER>(filter), IID_PPV_ARGS(&ppdl))))
	{
		unsigned uiCount = 0;
		ppdl->GetCount(&uiCount);
		for (unsigned i = 0; i < uiCount; ++i)
		{
			IPropertyDescription* ppd = nullptr;
			if (SUCCEEDED(ppdl->GetAt(i, IID_PPV_ARGS(&ppd))))
			{
				PROPERTYKEY key{};
				if (SUCCEEDED(ppd->GetPropertyKey(&key)))
				{
					wchar_t* pCanonicalName = nullptr;
					if (SUCCEEDED(ppd->GetCanonicalName(&pCanonicalName)) && pCanonicalName)
					{
						m_canonicalNames.push_back(pCanonicalName);
						m_keys.push_back(key);
						CoTaskMemFree(pCanonicalName);
					}
				}
				ppd->Release();
			}
		}
		ppdl->Release();
	}
	std::vector<String> hashPropertyNames;
	for (const auto& info : g_HashProperties)
		hashPropertyNames.push_back(info.pszCanonicalName);
	AddProperties(hashPropertyNames);
}

PropertySystem::PropertySystem(const std::vector<String>& canonicalNames)
{
	AddProperties(canonicalNames);
}

bool PropertySystem::AddProperty(const String& name)
{
	PROPERTYKEY key{};
	if (FAILED(PSGetPropertyKeyFromName(name.c_str(), &key)))
	{
		const PROPERTYKEY *pKey = GetPropertyKeyFromName(name);
		if (pKey)
		{
			key = *pKey;
			m_keys.push_back(key);
			m_canonicalNames.push_back(name);
			return true;
		}
	}
	else
	{
		m_keys.push_back(key);
		m_canonicalNames.push_back(name);
		m_onlyHashProperties = false;
		return true;
	}
	return false;
}

void PropertySystem::AddProperties(const std::vector<String>& canonicalNames)
{
	for (const auto& name : canonicalNames)
		AddProperty(name);
}

bool PropertySystem::GetPropertyValues(const String& path, PropertyValues& values)
{
	IPropertyStore* pps = nullptr;
	values.m_values.clear();
	if (!m_onlyHashProperties && SUCCEEDED(SHGetPropertyStoreFromParsingName(path.c_str(), nullptr, GPS_DEFAULT, IID_PPV_ARGS(&pps))))
	{
		values.m_values.reserve(m_keys.size());
		for (const auto& key : m_keys)
		{
			PROPVARIANT value{};
			if (GetPropertyIndexFromKey(key) >= 0)
				CalculateHashValue(path, key, value);
			else
				pps->GetValue(key, &value);
			values.m_values.push_back(value);
		}
		pps->Release();
		return true;
	}
	else
	{
		for (const auto& key : m_keys)
		{
			PROPVARIANT value2{};
			if (GetPropertyIndexFromKey(key) >= 0)
				CalculateHashValue(path, key, value2);
			values.m_values.push_back(value2);
		}
	}
	return false;
}

int PropertySystem::GetPropertyIndex(const String& canonicalName)
{
	auto it = std::find(m_canonicalNames.begin(), m_canonicalNames.end(), canonicalName);
	if (it != m_canonicalNames.end())
		return static_cast<int>(std::distance(m_canonicalNames.begin(), it));
	return -1;
}

bool PropertySystem::GetPropertyType(unsigned index, VARTYPE& vt) const
{
	if (index >= m_keys.size())
		return false;
	bool result = false;
	IPropertyDescription* ppd = nullptr;
	if (SUCCEEDED(PSGetPropertyDescription(m_keys[index], IID_PPV_ARGS(&ppd))))
	{
		if (SUCCEEDED(ppd->GetPropertyType(&vt)))
			result = true;
		ppd->Release();
	}
	return result;
}

String PropertySystem::FormatPropertyValue(const PropertyValues& values, unsigned index)
{
	if (index >= values.m_values.size())
		return _T("");
	String value;
	if (values.m_values[index].vt == (VT_VECTOR | VT_UI1))
	{
		const CAUB& buf = values.m_values[index].caub;
		for (unsigned i = 0; i < buf.cElems; ++i)
		{
			BYTE c = buf.pElems[i];
			value += "0123456789abcdef"[c >> 4];
			value += "0123456789abcdef"[c & 0xf];
		}
		return value;
	}
	PWSTR pszDisplayValue = NULL;
	HRESULT hr = PSFormatForDisplayAlloc(m_keys[index], values.m_values[index], PDFF_DEFAULT, &pszDisplayValue);
	if (SUCCEEDED(hr))
	{
		value = pszDisplayValue;
		CoTaskMemFree(pszDisplayValue);
	}
	return value;
}

bool PropertySystem::GetDisplayNames(std::vector<String>& names)
{
	for (const auto& key : m_keys)
	{
		IPropertyDescription* ppd = nullptr;
		if (SUCCEEDED(PSGetPropertyDescription(key, IID_PPV_ARGS(&ppd))))
		{
			wchar_t* pDisplayName = nullptr;
			if (SUCCEEDED(ppd->GetDisplayName(&pDisplayName)) && pDisplayName != nullptr)
			{
				names.push_back(pDisplayName);
				CoTaskMemFree(pDisplayName);
			}
			else
			{
				names.push_back(_T(""));
			}
			ppd->Release();
		}
		else
		{
			for (const auto& info : g_HashProperties)
			{
				if (*info.pKey == key)
					names.push_back(info.pszDisplayName);
			}
		}
	}
	return true;
}

bool PropertySystem::HasHashProperties() const
{
	for (const auto& key : m_keys)
	{
		if (GetPropertyIndexFromKey(key) >= 0)
			return true;
	}
	return false;
}

#else

PropertyValues::PropertyValues()
{
}

PropertyValues::~PropertyValues()
{
}

int PropertyValues::CompareValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index)
{
	return 0;
}

int64_t PropertyValues::DiffValues(const PropertyValues& values1, const PropertyValues& values2, unsigned index, bool& numeric)
{
	return 0;
}

int PropertyValues::CompareAllValues(const PropertyValues& values1, const PropertyValues& values2)
{
	return 0;
}

bool PropertyValues::IsEmptyValue(size_t index) const
{
	return false;
}

bool PropertyValues::IsHashValue(size_t index) const
{
	return false;
}

std::vector<uint8_t> PropertyValues::GetHashValue(size_t index) const
{
	return {};
}

PropertySystem::PropertySystem(ENUMFILTER filter)
{
}

PropertySystem::PropertySystem(const std::vector<String>& canonicalNames)
{
}

bool PropertySystem::GetPropertyValues(const String& path, PropertyValues& values)
{
	return false;
}

int PropertySystem::GetPropertyIndex(const String& canonicalName)
{
	return -1;
}

bool PropertySystem::GetPropertyType(unsigned index, VARTYPE& vt) const
{
	return false;
}

String PropertySystem::FormatPropertyValue(const PropertyValues& values, unsigned index)
{
	return _T("");
}

bool PropertySystem::GetDisplayNames(std::vector<String>& names)
{
	return false;
}

bool PropertySystem::HasHashProperties() const
{
	return false;
}

#endif