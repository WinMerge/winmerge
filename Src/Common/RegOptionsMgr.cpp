/** 
 * @file RegOptionsMgr.cpp
 *
 * @brief Implementation of Registry Options management class.
 *
 */

#include "pch.h"
#include "RegOptionsMgr.h"
#include <windows.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <vector>
#include "varprop.h"
#include "OptionsMgr.h"

#define MAX_PATH_FULL 32767

struct AsyncWriterThreadParams
{
	String name;
	varprop::VariantValue value;
};

CRegOptionsMgr::CRegOptionsMgr()
	: m_serializing(true)
	, m_bCloseHandle(false)
	, m_dwThreadId(0)
	, m_hThread(nullptr)
	, m_dwQueueCount(0)
{
	InitializeCriticalSection(&m_cs);
	m_hThread = CreateThread(nullptr, 0, AsyncWriterThreadProc, this, 0, &m_dwThreadId);
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

/**
 * @brief Split option name to path (in registry) and
 * valuename (in registry).
 *
 * Option names are given as "full path", e.g. "Settings/AutomaticRescan".
 * This function splits that to path "Settings/" and valuename
 * "AutomaticRescan".
 * @param [in] strName Option name
 * @param [out] srPath Path (key) in registry
 * @param [out] strValue Value in registry
 */
void CRegOptionsMgr::SplitName(const String &strName, String &strPath,
	String &strValue) const
{
	size_t pos = strName.rfind('/');
	if (pos != String::npos)
	{
		size_t len = strName.length();
		strValue = strName.substr(pos + 1, len - pos - 1); //Right(len - pos - 1);
		strPath = strName.substr(0, pos);  //Left(pos);
	}
	else
	{
		strValue = strName;
		strPath.erase();
	}
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

DWORD WINAPI CRegOptionsMgr::AsyncWriterThreadProc(void *pvThis)
{
	CRegOptionsMgr *pThis = reinterpret_cast<CRegOptionsMgr *>(pvThis);
	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		AsyncWriterThreadParams *pParam = reinterpret_cast<AsyncWriterThreadParams *>(msg.wParam);
		String strPath;
		String strValueName;
		pThis->SplitName(pParam->name, strPath, strValueName);
		EnterCriticalSection(&pThis->m_cs);
		HKEY hKey = pThis->OpenKey(strPath, true);
		SaveValueToReg(hKey, strValueName, pParam->value);
		pThis->CloseKey(hKey, strPath);
		LeaveCriticalSection(&pThis->m_cs);
		delete pParam;
		InterlockedDecrement(&pThis->m_dwQueueCount);
	}
	return 0;
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
	String strPath;
	String strValueName;
	LONG retValReg = 0;
	std::vector<BYTE> data;
	DWORD type = 0;
	DWORD size = 0;
	int valType = value.GetType();
	int retVal = COption::OPT_OK;

	SplitName(strName, strPath, strValueName);

	// Get type and size of value in registry
	retValReg = RegQueryValueEx(hKey, strValueName.c_str(), 0, &type,
		nullptr, &size);
	
	if (retValReg == ERROR_SUCCESS)
	{
		data.resize(size + sizeof(TCHAR), 0);

		// Get data
		retValReg = RegQueryValueEx(hKey, strValueName.c_str(),
			0, &type, &data[0], &size);
	}
	
	if (retValReg == ERROR_SUCCESS)
	{
		if (type == REG_SZ && valType == varprop::VT_STRING )
		{
			value.SetString((TCHAR *)&data[0]);
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
	}
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
					(LPBYTE)strVal.c_str(), (DWORD)(strVal.length() + 1) * sizeof(TCHAR));
		}
		else
		{
			TCHAR str[1] = {0};
			retValReg = RegSetValueEx(hKey, strValueName.c_str(), 0, REG_SZ,
					(LPBYTE)&str, 1 * sizeof(TCHAR));
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
	String strPath;
	String strValueName;
	SplitName(name, strPath, strValueName);

	// Open key.
	EnterCriticalSection(&m_cs);
	HKEY hKey = OpenKey(strPath, false);

	// Check previous value
	// This just checks if the value exists, LoadValueFromReg() below actually
	// loads the value.
	DWORD type = 0;
	DWORD size = MAX_PATH_FULL;
	LONG retValReg;
	if (hKey)
	{
		BYTE dataBuf[MAX_PATH_FULL] = {0};
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
		else if (retValReg == ERROR_SUCCESS || retValReg == ERROR_MORE_DATA)
		{
			varprop::VariantValue value(defaultValue);
			retVal = LoadValueFromReg(hKey, name, value);
			if (retVal == COption::OPT_OK)
				retVal = Set(name, value);
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

int CRegOptionsMgr::InitOption(const String& name, const TCHAR *defaultValue)
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
		AsyncWriterThreadParams *pParam = new AsyncWriterThreadParams();
		pParam->name = name;
		pParam->value = value;
		InterlockedIncrement(&m_dwQueueCount);
		PostThreadMessage(m_dwThreadId, WM_USER, (WPARAM)pParam, 0);
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
int CRegOptionsMgr::SaveOption(const String& name, const TCHAR *value)
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

	String strPath;
	String strValueName;

	SplitName(name, strPath, strValueName);

	if (!strValueName.empty())
	{
		retVal = COptionsMgr::RemoveOption(name);
	}
	else
	{
		for (auto it = m_optionsMap.begin(); it != m_optionsMap.end(); )
		{
			if (it->first.find(strPath) == 0)
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
	
	m_registryRoot = keyname;

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

/**
 * @brief Export options to file.
 *
 * This function enumerates through our options storage and saves
 * every option name and value to file.
 *
 * @param [in] filename Filename where optios are written.
 * @return
 * - COption::OPT_OK when succeeds
 * - COption::OPT_ERR when writing to the file fails
 */
int CRegOptionsMgr::ExportOptions(const String& filename, const bool bHexColor /*= false*/) const
{
	int retVal = COption::OPT_OK;
	OptionsMap::const_iterator optIter = m_optionsMap.begin();
	while (optIter != m_optionsMap.end() && retVal == COption::OPT_OK)
	{
		const String name(optIter->first);
		String strVal;
		varprop::VariantValue value = optIter->second.Get();
		if (value.GetType() == varprop::VT_BOOL)
		{
			if (value.GetBool())
				strVal = _T("1");
			else
				strVal = _T("0");
		}
		else if (value.GetType() == varprop::VT_INT)
		{
			if ( bHexColor && (strutils::makelower(name).find(String(_T("color"))) != std::string::npos) )
				strVal = strutils::format(_T("0x%06x"), value.GetInt());
			else
				strVal = strutils::to_str(value.GetInt());
		}
		else if (value.GetType() == varprop::VT_STRING)
		{
			strVal = value.GetString();
		}

		bool bRet = !!WritePrivateProfileString(_T("WinMerge"), name.c_str(),
				strVal.c_str(), filename.c_str());
		if (!bRet)
			retVal = COption::OPT_ERR;
		++optIter;
	}
	return retVal;
}

/**
 * @brief Import options from file.
 *
 * This function reads options values and names from given file and
 * updates values to our options storage. If valuename does not exist
 * already in options storage its is not created.
 *
 * @param [in] filename Filename where optios are written.
 * @return
 * - COption::OPT_OK when succeeds
 * - COption::OPT_NOTFOUND if file wasn't found or didn't contain values
 */
int CRegOptionsMgr::ImportOptions(const String& filename)
{
	int retVal = COption::OPT_OK;
	const int BufSize = 20480; // This should be enough for a long time..
	TCHAR buf[BufSize] = {0};
	auto oleTranslateColor = [](unsigned color) -> unsigned { return ((color & 0xffffff00) == 0x80000000) ? GetSysColor(color & 0x000000ff) : color; };

	// Query keys - returns NUL separated strings
	DWORD len = GetPrivateProfileString(_T("WinMerge"), nullptr, _T(""),buf, BufSize, filename.c_str());
	if (len == 0)
		return COption::OPT_NOTFOUND;

	TCHAR *pKey = buf;
	while (*pKey != '\0')
	{
		varprop::VariantValue value = Get(pKey);
		if (value.GetType() == varprop::VT_BOOL)
		{
			bool boolVal = GetPrivateProfileInt(_T("WinMerge"), pKey, 0, filename.c_str()) == 1;
			value.SetBool(boolVal);
			SaveOption(pKey, boolVal);
		}
		else if (value.GetType() == varprop::VT_INT)
		{
			int intVal = GetPrivateProfileInt(_T("WinMerge"), pKey, 0, filename.c_str());
			if (strutils::makelower(pKey).find(String(_T("color"))) != std::string::npos)
				intVal = static_cast<int>(oleTranslateColor(static_cast<unsigned>(intVal)));
			value.SetInt(intVal);
			SaveOption(pKey, intVal);
		}
		else if (value.GetType() == varprop::VT_STRING)
		{
			TCHAR strVal[MAX_PATH_FULL] = {0};
			GetPrivateProfileString(_T("WinMerge"), pKey, _T(""), strVal, MAX_PATH_FULL, filename.c_str());
			value.SetString(strVal);
			SaveOption(pKey, strVal);
		}
		Set(pKey, value);

		pKey += _tcslen(pKey);

		// Check: pointer is not past string end, and next char is not null
		// double NUL char ends the keynames string
		if ((pKey < buf + len) && (*(pKey + 1) != '\0'))
			pKey++;
	}
	return retVal;
}
