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

static const PROPERTYKEY PKEY_WINMERGE_HASH_SHA256 = {};
static const PROPERTYKEY PKEY_WINMERGE_HASH_SHA1 = {};
static const PROPERTYKEY PKEY_WINMERGE_HASH_MD5 = {};
static const PROPERTYKEY PKEY_WINMERGE_HASH_CRC32 = {};

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
	AddProperties({ _T("WinMerge.Hash.SHA1"), _T("WinMerge.Hash.SHA256"), _T("WinMerge.Hash.CRC32"), _T("WinMerge.Hash.MD5") });
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
			if (name == _T("WinMerge.Hash.SHA256"))
				key = PKEY_WINMERGE_HASH_SHA256;
			else if (name == _T("WinMerge.Hash.SHA1"))
				key = PKEY_WINMERGE_HASH_SHA1;
			else if (name == _T("WinMerge.Hash.MD5"))
				key = PKEY_WINMERGE_HASH_MD5;
			else if (name == _T("WinMerge.Hash.CRC32"))
				key = PKEY_WINMERGE_HASH_CRC32;
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
			String value;
			if (!GetPropertyString(pps, key, value))
			{
				if (key == PKEY_WINMERGE_HASH_SHA256)
				{
				}
				else if (key == PKEY_WINMERGE_HASH_SHA1)
				{
				}
				else if (key == PKEY_WINMERGE_HASH_MD5)
				{
				}
				else if (key == PKEY_WINMERGE_HASH_CRC32)
				{
				}
			}
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
		else if (key == PKEY_WINMERGE_HASH_SHA256)
			names.push_back(_T("SHA-256"));
		else if (key == PKEY_WINMERGE_HASH_SHA1)
			names.push_back(_T("SHA-1"));
		else if (key == PKEY_WINMERGE_HASH_MD5)
			names.push_back(_T("MD5"));
		else if (key == PKEY_WINMERGE_HASH_CRC32)
			names.push_back(_T("CRC32"));
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