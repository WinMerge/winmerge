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

static String GetPropertyString(IPropertyStore* pps, REFPROPERTYKEY key)
{
	String val;
	PROPVARIANT propvarValue = { 0 };
	HRESULT hr = pps->GetValue(key, &propvarValue);
	if (SUCCEEDED(hr))
	{
		PWSTR pszDisplayValue = NULL;
		hr = PSFormatForDisplayAlloc(key, propvarValue, PDFF_DEFAULT, &pszDisplayValue);
		if (SUCCEEDED(hr))
		{
			val = pszDisplayValue;
			CoTaskMemFree(pszDisplayValue);
		}
		PropVariantClear(&propvarValue);
	}
	return val;
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
}

PropertySystem::PropertySystem(const std::vector<String>& canonicalNames)
{
	m_canonicalNames = canonicalNames;
	for (const auto& name : canonicalNames)
	{
		PROPERTYKEY key{};
		PSGetPropertyKeyFromName(name.c_str(), &key);
		m_keys.push_back(key);
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
			values.push_back(GetPropertyString(pps, key));
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