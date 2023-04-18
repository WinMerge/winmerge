/** 
 * @file RegOptionsMgr.cpp
 *
 * @brief Implementation of Registry Options management class.
 *
 */

#include "pch.h"
#include "RegOptionsMgr.h"
#include <windows.h>
#include <process.h>
#include <Shlwapi.h>
#include "varprop.h"
#include "OptionsMgr.h"

constexpr size_t MAX_PATH_FULL = 32767;

struct AsyncWriterThreadParams
{
	AsyncWriterThreadParams(const String& name, const varprop::VariantValue& value) : name(name), value(value) {}
	String name;
	varprop::VariantValue value;
};

CRegOptionsMgr::CRegOptionsMgr()
	: m_serializing(true)
	, m_bCloseHandle(false)
	, m_dwThreadId(0)
	, m_hThread(nullptr)
	, m_hEvent(nullptr)
	, m_dwQueueCount(0)
{
	InitializeCriticalSection(&m_cs);
	m_hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (m_hEvent)
	{
		m_hThread = reinterpret_cast<HANDLE>(
			_beginthreadex(nullptr, 0, AsyncWriterThreadProc, this, 0,
				reinterpret_cast<unsigned*>(&m_dwThreadId)));
		WaitForSingleObject(m_hEvent, INFINITE);
		CloseHandle(m_hEvent);
		m_hEvent = nullptr;
	}
}

CRegOptionsMgr::~CRegOptionsMgr()
{
	for (;;) {
		PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
		if (WaitForSingleObject(m_hThread, 1) != WAIT_TIMEOUT)
			break;
	}
	DeleteCriticalSection(&m_cs);
}

HKEY CRegOptionsMgr::OpenKey(const String& strPath, bool bAlwaysCreate)
{
	String strRegPath(m_registryRoot);
	strRegPath += strPath;
	HKEY hKey = nullptr;
	if (m_hKeys.find(strPath) == m_hKeys.end())
	{
		DWORD action = 0;
		LONG retValReg;
		if (bAlwaysCreate)
		{
			retValReg = RegCreateKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
				0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
				&hKey, &action);
		}
		else
		{
			retValReg = RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
				0, KEY_ALL_ACCESS, &hKey);
		}
		if (retValReg != ERROR_SUCCESS)
			return nullptr;

		m_hKeys[strPath] = hKey;
	}
	else
	{
		hKey = m_hKeys[strPath];
	}
	return hKey;
}

void CRegOptionsMgr::CloseKey(HKEY hKey, const String& strPath)
{
	if (m_bCloseHandle)
	{
		if (hKey)
			RegCloseKey(hKey);
		m_hKeys.erase(strPath);
	}
}

void CRegOptionsMgr::CloseKeys()
{
	EnterCriticalSection(&m_cs);
	for (auto& pair : m_hKeys)
		RegCloseKey(pair.second);
	m_hKeys.clear();
	LeaveCriticalSection(&m_cs);
}

unsigned __stdcall CRegOptionsMgr::AsyncWriterThreadProc(void *pvThis)
{
	CRegOptionsMgr *pThis = reinterpret_cast<CRegOptionsMgr *>(pvThis);
	MSG msg;
	BOOL bRet;
	// create message queue
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(pThis->m_hEvent);
	while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		auto* pParam = reinterpret_cast<AsyncWriterThreadParams *>(msg.wParam);
		if (msg.message == WM_USER && pParam)
		{
			auto [strPath, strValueName] = COptionsMgr::SplitName(pParam->name);
			EnterCriticalSection(&pThis->m_cs);
			HKEY hKey = pThis->OpenKey(strPath, true);
			SaveValueToReg(hKey, strValueName, pParam->value);
			pThis->CloseKey(hKey, strPath);
			LeaveCriticalSection(&pThis->m_cs);
			delete pParam;
			InterlockedDecrement(&pThis->m_dwQueueCount);
		}
	}
	return 0;
}

int CRegOptionsMgr::LoadValueFromBuf(const String& strName, DWORD type, const BYTE* data, varprop::VariantValue& value)
{
	int retVal = COption::OPT_OK;
	int valType = value.GetType();
	if (type == REG_SZ && valType == varprop::VT_STRING )
	{
		value.SetString((tchar_t *)&data[0]);
		retVal = Set(strName, value);
	}
	else if (type == REG_DWORD)
	{
		if (valType == varprop::VT_INT)
		{
			DWORD dwordValue;
			CopyMemory(&dwordValue, &data[0], sizeof(DWORD));
			value.SetInt(dwordValue);
			retVal = Set(strName, value);
		}
		else if (valType == varprop::VT_BOOL)
		{
			DWORD dwordValue;
			CopyMemory(&dwordValue, &data[0], sizeof(DWORD));
			value.SetBool(dwordValue > 0 ? true : false);
			retVal = Set(strName, value);
		}
		else
			retVal = COption::OPT_WRONG_TYPE;
	}
	else
		retVal = COption::OPT_WRONG_TYPE;

	return retVal;
}

/**
 * @brief Load value from registry.
 *
 * Loads one value from registry from key previously opened. Type
 * is read from value parameter and error is returned if value
 * cannot be found or if it is different type than value parameter.
 * @param [in] hKey Handle to open registry key
 * @param [in] strName Name of value to read (incl. path!).
 * @param [in, out] value
 * [in] Values type must match to value type in registry
 * [out] Read value is returned
 * @note This function must handle ANSI and UNICODE data!
 * @todo Handles only string and integer types
 */
int CRegOptionsMgr::LoadValueFromReg(HKEY hKey, const String& strName,
	varprop::VariantValue &value)
{
	LONG retValReg = 0;
	std::vector<BYTE> data;
	DWORD type = 0;
	DWORD size = 0;
	int retVal = COption::OPT_OK;
	[[maybe_unused]] auto [strPath, strValueName] = SplitName(strName);

	// Get type and size of value in registry
	retValReg = RegQueryValueEx(hKey, strValueName.c_str(), 0, &type,
		nullptr, &size);
	
	if (retValReg == ERROR_SUCCESS)
	{
		data.resize(size + sizeof(tchar_t), 0);

		// Get data
		retValReg = RegQueryValueEx(hKey, strValueName.c_str(),
			0, &type, &data[0], &size);
	}
	
	if (retValReg == ERROR_SUCCESS)
		return LoadValueFromBuf(strName, type, data.data(), value);
	else
		retVal = COption::OPT_ERR;

	return retVal;
}

/**
 * @brief Save value to registry.
 *
 * Saves one value to registry to key previously opened. Type of
 * value is determined from given value parameter.
 * @param [in] hKey Handle to open registry key
 * @param [in] strValueName Name of value to write
 * @param [in] value value to write to registry value
 * @todo Handles only string and integer types
 */
int CRegOptionsMgr::SaveValueToReg(HKEY hKey, const String& strValueName,
	const varprop::VariantValue& value)
{
	LONG retValReg = 0;
	int valType = value.GetType();
	int retVal = COption::OPT_OK;

	if (valType == varprop::VT_STRING)
	{
		String strVal = value.GetString();
		if (strVal.length() > 0)
		{
			retValReg = RegSetValueEx(hKey, strValueName.c_str(), 0, REG_SZ,
					(LPBYTE)strVal.c_str(), (DWORD)(strVal.length() + 1) * sizeof(tchar_t));
		}
		else
		{
			tchar_t str[1] = {0};
			retValReg = RegSetValueEx(hKey, strValueName.c_str(), 0, REG_SZ,
					(LPBYTE)&str, 1 * sizeof(tchar_t));
		}
	}
	else if (valType == varprop::VT_INT)
	{
		DWORD dwordVal = value.GetInt();
		retValReg = RegSetValueEx(hKey, strValueName.c_str(), 0, REG_DWORD,
				(LPBYTE)&dwordVal, sizeof(DWORD));
	}
	else if (valType == varprop::VT_BOOL)
	{
		DWORD dwordVal = value.GetBool() ? 1 : 0;
		retValReg = RegSetValueEx(hKey, strValueName.c_str(), 0, REG_DWORD,
				(LPBYTE)&dwordVal, sizeof(DWORD));
	}
	else
	{
		retVal = COption::OPT_UNKNOWN_TYPE;
	}
		
	if (retValReg != ERROR_SUCCESS)
	{
		retVal = COption::OPT_ERR;
	}
	return retVal;
}

/**
 * @brief Init and add new option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(const String& name, const varprop::VariantValue& defaultValue)
{
	// Check type & bail if null
	int valType = defaultValue.GetType();
	if (valType == varprop::VT_NULL)
		return COption::OPT_ERR;

	// If we're not loading & saving options, bail
	if (!m_serializing)
		return AddOption(name, defaultValue);

	// Figure out registry path, for saving value
	auto [strPath, strValueName] = SplitName(name);

	// Open key.
	EnterCriticalSection(&m_cs);
	HKEY hKey = OpenKey(strPath, false);

	// Check previous value
	// This just checks if the value exists, LoadValueFromReg() below actually
	// loads the value.
	DWORD type = 0;
	BYTE dataBuf[MAX_PATH_FULL];
	DWORD size = sizeof(dataBuf);
	LONG retValReg;
	if (hKey)
	{
		dataBuf[0] = 0;
		retValReg = RegQueryValueEx(hKey, strValueName.c_str(),
			0, &type, dataBuf, &size);
	}
	else
		retValReg = ERROR_FILE_NOT_FOUND;

	// Actually save value into our in-memory options table
	int retVal = AddOption(name, defaultValue);
	
	// Update registry if successfully saved to in-memory table
	if (retVal == COption::OPT_OK)
	{
		// Value didn't exist. Do nothing
		if (retValReg == ERROR_FILE_NOT_FOUND)
		{
		}
		// Value already exists so read it.
		else if (retValReg == ERROR_SUCCESS)
		{
			varprop::VariantValue value(defaultValue);
			retVal = LoadValueFromBuf(name, type, dataBuf, value);
		}
		else if (retValReg == ERROR_MORE_DATA)
		{
			varprop::VariantValue value(defaultValue);
			retVal = LoadValueFromReg(hKey, name, value);
		}
	}

	CloseKey(hKey, strPath);
	LeaveCriticalSection(&m_cs);
	return retVal;
}

/**
 * @brief Init and add new string option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(const String& name, const String& defaultValue)
{
	varprop::VariantValue defValue;
	defValue.SetString(defaultValue);
	return InitOption(name, defValue);
}

int CRegOptionsMgr::InitOption(const String& name, const tchar_t *defaultValue)
{
	return InitOption(name, String(defaultValue));
}

/**
 * @brief Init and add new int option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(const String& name, int defaultValue, bool serializable)
{
	varprop::VariantValue defValue;
	int retVal = COption::OPT_OK;

	defValue.SetInt(defaultValue);
	if (serializable)
		retVal = InitOption(name, defValue);
	else
		AddOption(name, defValue);
	return retVal;
}

/**
 * @brief Init and add new boolean option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(const String& name, bool defaultValue)
{
	varprop::VariantValue defValue;
	defValue.SetBool(defaultValue);
	return InitOption(name, defValue);
}

/**
 * @brief Save option to registry
 * @note Currently handles only integer and string options!
 */
int CRegOptionsMgr::SaveOption(const String& name)
{
	if (!m_serializing) return COption::OPT_OK;

	varprop::VariantValue value;
	int retVal = COption::OPT_OK;

	value = Get(name);
	int valType = value.GetType();
	if (valType == varprop::VT_NULL)
		retVal = COption::OPT_NOTFOUND;

	if (retVal == COption::OPT_OK)
	{
		auto* pParam = new AsyncWriterThreadParams(name, value);
		InterlockedIncrement(&m_dwQueueCount);
		if (!PostThreadMessage(m_dwThreadId, WM_USER, (WPARAM)pParam, 0))
			InterlockedDecrement(&m_dwQueueCount);
	}
	return retVal;
}

/**
 * @brief Set new value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(const String& name, const varprop::VariantValue& value)
{
	int retVal = Set(name, value);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new string value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(const String& name, const String& value)
{
	varprop::VariantValue val;
	val.SetString(value);
	int retVal = Set(name, val);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new string value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(const String& name, const tchar_t *value)
{
	return SaveOption(name, String(value));
}

/**
 * @brief Set new integer value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(const String& name, int value)
{
	varprop::VariantValue val;
	val.SetInt(value);
	int retVal = Set(name, val);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new boolean value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(const String& name, bool value)
{
	varprop::VariantValue val;
	val.SetBool(value);
	int retVal = Set(name, val);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

int CRegOptionsMgr::RemoveOption(const String& name)
{
	int retVal = COption::OPT_OK;
	auto [strPath, strValueName] = SplitName(name);

	if (!strValueName.empty())
	{
		retVal = COptionsMgr::RemoveOption(name);
	}
	else
	{
		for (auto it = m_optionsMap.begin(); it != m_optionsMap.end(); )
		{
			const String& key = it->first;
			if (key.find(strPath) == 0 && key.length() > strPath.length() && key[strPath.length()] == '/')
				it = m_optionsMap.erase(it);
			else 
				++it;
		}
		retVal = COption::OPT_OK;
	}

	while (InterlockedCompareExchange(&m_dwQueueCount, 0, 0) != 0)
		Sleep(0);

	EnterCriticalSection(&m_cs);
	HKEY hKey = OpenKey(strPath, true);
	if (strValueName.empty())
#ifdef _WIN64
		RegDeleteTree(hKey, nullptr);
#else
		SHDeleteKey(hKey, nullptr);
#endif
	else
		RegDeleteValue(hKey, strValueName.c_str());
	CloseKey(hKey, strPath);
	LeaveCriticalSection(&m_cs);

	return retVal;
}

int CRegOptionsMgr::FlushOptions()
{
	int retVal = COption::OPT_OK;

	while (InterlockedCompareExchange(&m_dwQueueCount, 0, 0) != 0)
		Sleep(0);

	return retVal;
}

int CRegOptionsMgr::ExportOptions(const String& filename, const bool bHexColor /*= false*/) const
{
	HKEY hKey = nullptr;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, m_registryRoot.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return COption::OPT_ERR;
	int retVal = ExportAllUnloadedValues(hKey, _T(""), filename);
	if (retVal == COption::OPT_OK)
		retVal = COptionsMgr::ExportOptions(filename, bHexColor);
	RegCloseKey(hKey);
	return retVal;
}

int CRegOptionsMgr::ImportOptions(const String& filename)
{
	int retVal = ImportAllUnloadedValues(filename);
	if (retVal == COption::OPT_OK)
		retVal = COptionsMgr::ImportOptions(filename);
	return retVal;
}

int CRegOptionsMgr::ExportAllUnloadedValues(HKEY hKey, const String& strPath, const String& filename) const
{
	DWORD dwIndex = 0;
	tchar_t szValueName[MAX_PATH];
	std::vector<BYTE> data(MAX_PATH);
	for (;;)
	{
		DWORD dwType;
		DWORD cbValueName = MAX_PATH;
		DWORD cbData = static_cast<DWORD>(data.size());
		LSTATUS result = RegEnumValue(hKey, dwIndex, szValueName, &cbValueName, nullptr, &dwType, data.data(), &cbData);
		if (result == ERROR_MORE_DATA)
		{
			cbValueName = MAX_PATH;
			cbData *= 2;
			data.resize(cbData);
			result = RegEnumValue(hKey, dwIndex, szValueName, &cbValueName, nullptr, &dwType, data.data(), &cbData);
		}
		if (result == ERROR_SUCCESS)
		{
			String strName = strPath + _T("/") + szValueName;
			if (m_optionsMap.find(strName) == m_optionsMap.end())
			{
				varprop::VariantValue value;
				switch (dwType)
				{
				case REG_DWORD:
					WritePrivateProfileString(_T("WinMerge"), strName.c_str(),
						strutils::to_str(*(reinterpret_cast<int*>(data.data()))).c_str(), filename.c_str());
					WritePrivateProfileString(_T("WinMerge.TypeInfo"), strName.c_str(),
							_T("int"), filename.c_str());
					break;
				case REG_SZ:
					WritePrivateProfileString(_T("WinMerge"), strName.c_str(),
						EscapeValue(reinterpret_cast<tchar_t*>(data.data())).c_str(), filename.c_str());
					WritePrivateProfileString(_T("WinMerge.TypeInfo"), strName.c_str(),
							_T("string"), filename.c_str());
					break;
				default:
					break;
				}
			}
			dwIndex++;
		}
		else if (result == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		else
		{
			return COption::OPT_ERR;
		}
	}

	dwIndex = 0;
	tchar_t szSubKey[MAX_PATH];
	DWORD cbSubKey = MAX_PATH;
	for (;;)
	{
		LSTATUS result = RegEnumKeyEx(hKey, dwIndex, szSubKey, &cbSubKey, nullptr, nullptr, nullptr, nullptr);
		if (result == ERROR_SUCCESS)
		{
			HKEY hSubKey = nullptr;
			if (RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
			{
				int retVal = ExportAllUnloadedValues(hSubKey, strPath.empty() ? szSubKey : strPath + _T("\\") + szSubKey, filename);
				RegCloseKey(hSubKey);
				if (retVal != COption::OPT_OK)
					return retVal;
			}
			dwIndex++;
			cbSubKey = MAX_PATH;
		}
		else if (result == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		else
		{
			return COption::OPT_ERR;
		}
	}
	return COption::OPT_OK;
}

int CRegOptionsMgr::ImportAllUnloadedValues(const String& filename)
{
	auto iniFileKeyValues = ReadIniFile(filename, _T("WinMerge"));
	auto iniFileKeyTypes = ReadIniFile(filename, _T("WinMerge.TypeInfo"));
	for (const auto& [key, strValue] : iniFileKeyValues)
	{
		if (m_optionsMap.find(key) == m_optionsMap.end() &&
		    iniFileKeyTypes.find(key) != iniFileKeyTypes.end())
		{
			auto [strPath, strValueName] = SplitName(key);
			HKEY hKey = OpenKey(strPath, true);
			if (hKey)
			{
				varprop::VariantValue value;
				String strType = iniFileKeyTypes[key];
				if (tc::tcsicmp(strType.c_str(), _T("bool")) == 0)
					value.SetBool(tc::ttoi(strValue.c_str()) != 0);
				else if (tc::tcsicmp(strType.c_str(), _T("int")) == 0)
				{
					tchar_t* endptr = nullptr;
					unsigned uval = static_cast<unsigned>(tc::tcstoll(strValue.c_str(), &endptr,
						(strValue.length() >= 2 && strValue[1] == 'x') ? 16 : 10));
					value.SetInt(static_cast<int>(uval));
				}
				else if (tc::tcsicmp(strType.c_str(), _T("string")) == 0)
					value.SetString(strValue);
				SaveValueToReg(hKey, strValueName, value);
				CloseKey(hKey, strPath);
			}
		}
	}
	return COption::OPT_OK;
}

/**
 * @brief Set registry root path for options.
 *
 * Sets path used as root path when loading/saving options. Paths
 * given to other functions are relative to this path.
 */
int CRegOptionsMgr::SetRegRootKey(const String& key)
{
	String keyname(key);
	HKEY hKey = nullptr;
	DWORD action = 0;
	int retVal = COption::OPT_OK;

	size_t ind = keyname.find(_T("Software"));
	if (ind != 0)
		keyname.insert(0, _T("Software\\"));
	
	m_registryRoot = std::move(keyname);

	LONG retValReg =  RegCreateKeyEx(HKEY_CURRENT_USER, m_registryRoot.c_str(), 0, nullptr,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, &action);

	if (retValReg == ERROR_SUCCESS)
	{
		if (action == REG_CREATED_NEW_KEY)
		{
			// TODO: At least log message..?
		}
		RegCloseKey(hKey);
	}
	else
	{
		retVal = COption::OPT_ERR;
	}

	return retVal;
}
