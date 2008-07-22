/** 
 * @file RegOptionsMgr.cpp
 *
 * @brief Implementation of Registry Options management class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

// Disable VC6's "identifier was truncated..." warning. It is meaningless.
#if _MSC_VER == 1200
	#pragma warning(disable: 4786)
#endif
#include <windows.h>
#include <tchar.h>
#include "OptionsMgr.h"
#include "RegOptionsMgr.h"

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
void CRegOptionsMgr::SplitName(String strName, String &strPath,
	String &strValue)
{
	int pos = strName.rfind('/');
	if (pos > 0)
	{
		int len = strName.length();
		strValue = strName.substr(pos + 1, len - pos - 1); //Right(len - pos - 1);
		strPath = strName.substr(0, pos);  //Left(pos);
	}
	else
	{
		strValue = strName;
		strPath.erase();
	}
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
int CRegOptionsMgr::LoadValueFromReg(HKEY hKey, LPCTSTR strName,
	varprop::VariantValue &value)
{
	String strPath;
	String strValueName;
	LONG retValReg = 0;
	LPBYTE pData = NULL;
	DWORD type = 0;
	TCHAR * valueBuf = NULL;
	DWORD size = 0;
	int valType = value.GetType();
	int retVal = OPT_OK;

	SplitName(strName, strPath, strValueName);

	// Get type and size of value in registry
	retValReg = RegQueryValueEx(hKey, strValueName.c_str(), 0, &type,
		NULL, &size);
	
	if (retValReg == ERROR_SUCCESS)
	{
		pData = new BYTE[size + sizeof(TCHAR)];
		if (pData == NULL)
			return OPT_ERR;
		ZeroMemory(pData, size + sizeof(TCHAR));

		// Get data
		retValReg = RegQueryValueEx(hKey, strValueName.c_str(),
			0, &type, pData, &size);
	}
	
	if (retValReg == ERROR_SUCCESS)
	{
		if (type == REG_SZ && valType == varprop::VT_STRING )
		{
			value.SetString((LPCTSTR)pData);
			retVal = Set(strName, value);
		}
		else if (type == REG_DWORD)
		{
			if (valType == varprop::VT_INT)
			{
				DWORD dwordValue;
				CopyMemory(&dwordValue, pData, sizeof(DWORD));
				value.SetInt(dwordValue);
				retVal = Set(strName, value);
			}
			else if (valType == varprop::VT_BOOL)
			{
				DWORD dwordValue;
				CopyMemory(&dwordValue, pData, sizeof(DWORD));
				value.SetBool(dwordValue > 0 ? true : false);
				retVal = Set(strName, value);
			}
			else
				retVal = OPT_WRONG_TYPE;
		}
		else
			retVal = OPT_WRONG_TYPE;
	}
	else
		retVal = OPT_ERR;

	delete [] pData;

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
int CRegOptionsMgr::SaveValueToReg(HKEY hKey, LPCTSTR strValueName,
	varprop::VariantValue value)
{
	LONG retValReg = 0;
	int valType = value.GetType();
	int retVal = OPT_OK;

	if (valType == varprop::VT_STRING)
	{
		String strVal = value.GetString();
		if (strVal.length() > 0)
		{
			retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_SZ,
					(LPBYTE)strVal.c_str(), (strVal.length() + 1) * sizeof(TCHAR));
		}
		else
		{
			TCHAR str[1] = {0};
			retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_SZ,
					(LPBYTE)&str, 1 * sizeof(TCHAR));
		}
	}
	else if (valType == varprop::VT_INT)
	{
		DWORD dwordVal = value.GetInt();
		retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_DWORD,
				(LPBYTE)&dwordVal, sizeof(DWORD));
	}
	else if (valType == varprop::VT_BOOL)
	{
		DWORD dwordVal = value.GetBool() ? 1 : 0;
		retValReg = RegSetValueEx(hKey, (LPCTSTR)strValueName, 0, REG_DWORD,
				(LPBYTE)&dwordVal, sizeof(DWORD));
	}
	else
	{
		retVal = OPT_UNKNOWN_TYPE;
	}
		
	if (retValReg != ERROR_SUCCESS)
	{
		retVal = OPT_ERR;
	}
	return retVal;
}

/**
 * @brief Init and add new option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(LPCTSTR name, varprop::VariantValue defaultValue)
{
	// Check type & bail if null
	int valType = defaultValue.GetType();
	if (valType == varprop::VT_NULL)
		return OPT_ERR;

	// If we're not loading & saving options, bail
	if (!m_serializing)
		return AddOption(name, defaultValue);

	// Figure out registry path, for saving value
	String strPath;
	String strValueName;
	SplitName(name, strPath, strValueName);
	String strRegPath(m_registryRoot);
	strRegPath += strPath;

	// Open key. Create new key if it does not exist.
	HKEY hKey = NULL;
	DWORD action = 0;
	LONG retValReg = RegCreateKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
		NULL, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
		&hKey, &action);

	if (retValReg != ERROR_SUCCESS)
		return OPT_ERR;

	// Check previous value
	// This just checks if the value exists, LoadValueFromReg() below actually
	// loads the value.
	DWORD type = 0;
	BYTE dataBuf[MAX_PATH] = {0};
	DWORD size = MAX_PATH;
	retValReg = RegQueryValueEx(hKey, strValueName.c_str(),
		0, &type, dataBuf, &size);

	// Actually save value into our in-memory options table
	int retVal = AddOption(name, defaultValue);
	
	// Update registry if successfully saved to in-memory table
	if (retVal == OPT_OK)
	{
		// Value didn't exist. Save default value to registry
		if (retValReg == ERROR_FILE_NOT_FOUND)
		{
			retVal = SaveValueToReg(hKey, strValueName.c_str(), defaultValue);
		}
		// Value already exists so read it.
		else if (retValReg == ERROR_SUCCESS || ERROR_MORE_DATA)
		{
			retVal = LoadValueFromReg(hKey, name, defaultValue);
			if (retVal == OPT_OK)
				retVal = Set(name, defaultValue);
		}
	}
	RegCloseKey(hKey);
	return retVal;
}

/**
 * @brief Init and add new string option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(LPCTSTR name, LPCTSTR defaultValue)
{
	varprop::VariantValue defValue;
	int retVal = OPT_OK;
	
	defValue.SetString(defaultValue);
	retVal = InitOption(name, defValue);
	return retVal;
}

/**
 * @brief Init and add new int option.
 *
 * Adds new option to list of options. Sets value to default value.
 * If option does not exist in registry, saves with default value.
 */
int CRegOptionsMgr::InitOption(LPCTSTR name, int defaultValue, bool serializable)
{
	varprop::VariantValue defValue;
	int retVal = OPT_OK;
	
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
int CRegOptionsMgr::InitOption(LPCTSTR name, bool defaultValue)
{
	varprop::VariantValue defValue;
	int retVal = OPT_OK;
	
	defValue.SetBool(defaultValue);
	retVal = InitOption(name, defValue);
	return retVal;
}

/**
 * @brief Load option from registry.
 * @note Currently handles only integer and string options!
 */
int CRegOptionsMgr::LoadOption(LPCTSTR name)
{
	varprop::VariantValue value;
	String strPath;
	String strValueName;
	String strRegPath(m_registryRoot);
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int valType = varprop::VT_NULL;
	int retVal = OPT_OK;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	value = Get(name);
	valType = value.GetType();
	if (valType == varprop::VT_NULL)
		retVal = OPT_NOTFOUND;
	
	if (retVal == OPT_OK)
	{
		retValReg = RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
			NULL, KEY_READ, &hKey);

		if (retValReg == ERROR_SUCCESS)
		{
			retVal = LoadValueFromReg(hKey, name, value);
			RegCloseKey(hKey);
		}
		else
			retVal = OPT_ERR;
	}
	return retVal;
}

/**
 * @brief Save option to registry
 * @note Currently handles only integer and string options!
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name)
{
	if (!m_serializing) return OPT_OK;

	varprop::VariantValue value;
	String strPath;
	String strValueName;
	String strRegPath(m_registryRoot);
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int valType = varprop::VT_NULL;
	int retVal = OPT_OK;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	value = Get(name);
	valType = value.GetType();
	if (valType == varprop::VT_NULL)
		retVal = OPT_NOTFOUND;
	
	if (retVal == OPT_OK)
	{
		retValReg = RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(),
			NULL, KEY_WRITE, &hKey);

		if (retValReg == ERROR_SUCCESS)
		{
			retVal = SaveValueToReg(hKey, strValueName.c_str(), value);
			RegCloseKey(hKey);
		}
		else
			retVal = OPT_ERR;
	}
	return retVal;
}

/**
 * @brief Set new value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, varprop::VariantValue value)
{
	int retVal = OPT_OK;
	retVal = Set(name, value);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new string value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, LPCTSTR value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetString(value);
	retVal = Set(name, val);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new integer value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, int value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetInt(value);
	retVal = Set(name, val);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new boolean value for option and save option to registry
 */
int CRegOptionsMgr::SaveOption(LPCTSTR name, bool value)
{
	varprop::VariantValue val;
	int retVal = OPT_OK;

	val.SetBool(value);
	retVal = Set(name, val);
	if (retVal == OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

int CRegOptionsMgr::RemoveOption(LPCTSTR name)
{
	HKEY hKey = NULL;
	LONG retValReg = 0;
	int retVal = OPT_OK;
	String strRegPath(m_registryRoot);
	String strPath;
	String strValueName;

	SplitName(name, strPath, strValueName);
	strRegPath += strPath;

	retValReg = RegOpenKey(HKEY_CURRENT_USER, strRegPath.c_str(), &hKey);
	if (retValReg == ERROR_SUCCESS)
	{
		retValReg = RegDeleteValue(hKey, strValueName.c_str());
		if (retValReg != ERROR_SUCCESS)
		{
			retVal = OPT_ERR;
		}
		RegCloseKey(hKey);
	}
	else
		retVal = OPT_ERR;

	return retVal;

}

/**
 * @brief Set registry root path for options.
 *
 * Sets path used as root path when loading/saving options. Paths
 * given to other functions are relative to this path.
 */
int CRegOptionsMgr::SetRegRootKey(LPCTSTR key)
{
	String keyname(key);
	HKEY hKey = NULL;
	LONG retValReg = 0;
	DWORD action = 0;
	int retVal = OPT_OK;
	int ind = 0;

	ind = keyname.find(_T("Software"));
	if (ind != 0)
		keyname.insert(0, _T("Software\\"));
	
	m_registryRoot = keyname;

	retValReg =  RegCreateKeyEx(HKEY_CURRENT_USER, m_registryRoot.c_str(), NULL, _T(""),
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &action);

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
		retVal = OPT_ERR;
	}

	return retVal;
}
