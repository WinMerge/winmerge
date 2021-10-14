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
#pragma comment(lib, "bcrypt.lib")
 
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

static NTSTATUS CalculateHashValue(HANDLE hFile, BCRYPT_HASH_HANDLE hHash, ULONG hashSize, std::vector<uint8_t>& hash)
{
	hash.resize(hashSize);
	std::vector<uint8_t> buffer(769);
	NTSTATUS status = 0;
	while (status == 0)
	{
		DWORD dwRead = 0;
		if (!ReadFile(hFile, buffer.data(), static_cast<DWORD>(buffer.size()), &dwRead, nullptr))
		{
			status = 1; // STATUS_UNSUCCESSFUL
			break;
		}
		status = BCryptHashData(hHash, buffer.data(), dwRead, 0);
		if (buffer.size() != dwRead)
			break;
	}
	if (status == 0)
	{
		status = BCryptFinishHash(hHash, hash.data(), static_cast<ULONG>(hash.size()), 0);
	}
	return status;
}

static NTSTATUS CalculateHashValue(HANDLE hFile, const wchar_t *pAlgoId, std::vector<uint8_t>& hash)
{
	hash.clear();
	BCRYPT_ALG_HANDLE hAlg = nullptr;
	NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, pAlgoId, nullptr, 0);
	if (status == 0)
	{
		ULONG bytesWritten = 0;
		ULONG objectSize = 0;
		status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectSize), sizeof(DWORD), &bytesWritten, 0);
		if (status == 0)
		{
			std::vector<uint8_t> hashObject(objectSize);
			BCRYPT_HASH_HANDLE hHash = nullptr;
			status = BCryptCreateHash(hAlg, &hHash, hashObject.data(), static_cast<ULONG>(hashObject.size()), nullptr, 0, 0);
			if (status == 0)
			{
				ULONG hashSize = 0;
				status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&hashSize), sizeof(DWORD), &bytesWritten, 0);
				if (status == 0)
				{
					status = CalculateHashValue(hFile, hHash, hashSize, hash);
				}
				BCryptDestroyHash(hHash);
			}
		}
		BCryptCloseAlgorithmProvider(hAlg, 0);
	}
	return status;
}

static String CalculateHashValue(const String& path, const PROPERTYKEY& key)
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
		String hashString;
		for (auto c : hash)
		{
			hashString += L"0123456789abcdef"[c >> 4];
			hashString += L"0123456789abcdef"[c & 0xf];
		}
		return hashString;
	}
	return _T("");
}

static String GetPropertyString(IPropertyStore* pps, const PROPERTYKEY& key)
{
	String value;
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
	return value;
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
			const PROPERTYKEY *pKey = GetPropertyKeyFromName(name);
			if (pKey)
				key = *pKey;
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
			String value = GetPropertyIndexFromKey(key) >= 0 ?
				CalculateHashValue(path, key) : GetPropertyString(pps, key);
			values.push_back(value);
		}
		pps->Release();
		return true;
	}
	else
	{
		for (const auto& key : m_keys)
			values.push_back(_T("Error"));
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