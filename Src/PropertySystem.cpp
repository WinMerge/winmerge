/** 
 * @file  PropertySystem.cpp
 *
 * @brief Implementation file for PropertySystem
 */
#include "pch.h"
#include "PropertySystem.h"
#ifdef _WIN64
#include <shobjidl.h>
#include <propsys.h>
#include <propvarutil.h>
#include <propkey.h>
#include <strsafe.h>
#endif

#ifdef _WIN64

#pragma comment(lib, "propsys.lib")
 
 // {ECA2D096-7C87-4DFF-94E7-E7FE9BA34BE8} 100-102
static const PROPERTYKEY PKEY_WINMERGE_HASH_SHA256 = { {0xeca2d096, 0x7c87, 0x4dff, 0x94, 0xe7, 0xe7, 0xfe, 0x9b, 0xa3, 0x4b, 0xe8}, 100 };
static const PROPERTYKEY PKEY_WINMERGE_HASH_SHA1 ={ {0xeca2d096, 0x7c87, 0x4dff, 0x94, 0xe7, 0xe7, 0xfe, 0x9b, 0xa3, 0x4b, 0xe8}, 101 }; 
static const PROPERTYKEY PKEY_WINMERGE_HASH_MD5 = { {0xeca2d096, 0x7c87, 0x4dff, 0x94, 0xe7, 0xe7, 0xfe, 0x9b, 0xa3, 0x4b, 0xe8}, 102 };

struct PROPERTYINFO
{
	const PROPERTYKEY* pKey;
	const wchar_t *pszCanonicalName;
	const wchar_t *pszDisplayName;
};

static const PROPERTYINFO g_HashProperties[] =
{
	{ &PKEY_WINMERGE_HASH_SHA256, L"WinMerge.Hash.SHA256", L"SHA256" },
	{ &PKEY_WINMERGE_HASH_SHA1,   L"WinMerge.Hash.SHA1",   L"SHA1" },
	{ &PKEY_WINMERGE_HASH_MD5,    L"WinMerge.Hash.MD5",    L"MD5" },
};

static String CalculateHashValue(const String& path, const PROPERTYKEY& key)
{
	return _T("TEST");
}

static bool GetPropertyString(IPropertyStore* pps, const PROPERTYKEY& key, String& value)
{
	PROPVARIANT propvarValue = { 0 };
	HRESULT hr = pps->GetValue(key, &propvarValue);
	if (SUCCEEDED(hr))
	{
		PWSTR pszDisplayValue = NULL;
		hr = PSFormatForDisplayAlloc(key, propvarValue, PDFF_DEFAULT, &pszDisplayValue);
		if (SUCCEEDED(hr))
		{
			value = pszDisplayValue;
			CoTaskMemFree(pszDisplayValue);
		}
		PropVariantClear(&propvarValue);
	}
	return SUCCEEDED(hr);
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

void PropertySystem::AddProperties(const std::vector<String>& canonicalNames)
{
	for (const auto& name : canonicalNames)
	{
		PROPERTYKEY key{};
		if (FAILED(PSGetPropertyKeyFromName(name.c_str(), &key)))
		{
			for (const auto& info : g_HashProperties)
			{
				if (info.pszCanonicalName == name)
				{
					key = *info.pKey;
					break;
				}
			}
		}
		m_keys.push_back(key);
		m_canonicalNames.push_back(name);
	}
}

bool PropertySystem::GetFormattedValues(const String& path, std::vector<String>& values)
{
	IPropertyStore* pps = nullptr;
	values.clear();
	if (SUCCEEDED(SHGetPropertyStoreFromParsingName(path.c_str(), nullptr, GPS_DEFAULT, IID_PPV_ARGS(&pps))))
	{
		values.reserve(m_keys.size());
		for (const auto& key : m_keys)
		{
			bool found = false;
			String value;
			for (const auto& info : g_HashProperties)
			{
				if (*info.pKey == key)
				{
					value = CalculateHashValue(path, key);
					found = true;
					break;
				}
			}
			if (!found)
				GetPropertyString(pps, key, value);
			values.push_back(value);
		}
		pps->Release();
		return true;
	}
	else
	{
		for (const auto& key : m_keys)
			values.push_back(_T("error"));
	}
	return false;
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
			else if (SUCCEEDED(ppd->GetCanonicalName(&pDisplayName)) && pDisplayName != nullptr)
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

#else

PropertySystem::PropertySystem(ENUMFILTER filter)
{
}

PropertySystem::PropertySystem(const std::vector<String>& canonicalNames)
{
}

bool PropertySystem::GetFormattedValues(const String& path, std::vector<String>& values)
{
	return false;
}

bool PropertySystem::GetDisplayNames(std::vector<String>& names)
{
	return false;
}


#endif