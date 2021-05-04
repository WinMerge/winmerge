/**
 * @file IniOptionsMgr.cpp
 *
 * @brief Implementation of Ini file Options management class.
 *
 */

#include "pch.h"
#include "IniOptionsMgr.h"
#include "OptionsMgr.h"
#include <Windows.h>
#include <codecvt>
#include <filesystem>
#include <string>
#include <fstream>

using std::filesystem::current_path;

LPCWSTR CIniOptionsMgr::lpFilePath = NULL;

LPCWSTR lpAppName = TEXT("WinMerge");
LPCWSTR lpFileName = TEXT("\\winmerge.ini");

CIniOptionsMgr::CIniOptionsMgr()
	: m_serializing(true)
{
	InitializeCriticalSection(&m_cs);
}

CIniOptionsMgr::~CIniOptionsMgr()
{
	DeleteCriticalSection(&m_cs);
	delete[] CIniOptionsMgr::lpFilePath;
}

/**
 * @brief Checks wheter INI file exists.
 * @return TRUE if INI file exist,
 *   FALSE otherwise.
 */
bool CIniOptionsMgr::CheckIfIniFileExist()
{
	std::ifstream f(GetFilePath());
	return f.good();
}

/**
 * @brief Get path to INI file.
 * @return path to INI file
 */
LPCWSTR CIniOptionsMgr::GetFilePath()
{
	if (CIniOptionsMgr::lpFilePath == NULL)
	{
		// create path
		std::filesystem::path p = current_path();
		p += lpFileName;

		// change type
		std::wstring str = p.wstring();
		size_t length = str.length() + 1;
		wchar_t* strCp = new wchar_t[length];
		wcscpy_s(strCp, length, str.c_str());

		// set path
		CIniOptionsMgr::lpFilePath = strCp;
	}

	return CIniOptionsMgr::lpFilePath;
}

int CIniOptionsMgr::InitOption(const String& name, const varprop::VariantValue& defaultValue)
{
	// Check type & bail if null
	int valType = defaultValue.GetType();
	if (valType == varprop::VT_NULL)
		return COption::OPT_ERR;

	// If we're not loading & saving options, bail
	if (!m_serializing)
		return AddOption(name, defaultValue);

	EnterCriticalSection(&m_cs);

	// check if value exist
	String textValue = ReadValueFromFile(name);
	bool found = textValue.size() != 0;

	// Actually save value into our in-memory options table
	int retVal = AddOption(name, defaultValue);

	// Update registry if successfully saved to in-memory table
	if (retVal == COption::OPT_OK)
	{
		if (found)
		{
			varprop::VariantValue value(defaultValue);
			retVal = ParseValue(name, textValue, value);
			if (retVal == COption::OPT_OK)
			{
				retVal = Set(name, value);
			}
		}
	}

	LeaveCriticalSection(&m_cs);
	return retVal;
}

int CIniOptionsMgr::InitOption(const String& name, const String& defaultValue)
{
	varprop::VariantValue defValue;
	defValue.SetString(defaultValue);
	return InitOption(name, defValue);
}

int CIniOptionsMgr::InitOption(const String& name, const TCHAR* defaultValue)
{
	return InitOption(name, String(defaultValue));
}

int CIniOptionsMgr::InitOption(const String& name, int defaultValue, bool serializable)
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

int CIniOptionsMgr::InitOption(const String& name, bool defaultValue)
{
	varprop::VariantValue defValue;
	defValue.SetBool(defaultValue);
	return InitOption(name, defValue);
}

int CIniOptionsMgr::SaveOption(const String& name)
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
		if (valType == varprop::VT_STRING)
		{
			String strVal = value.GetString();
			LPCWSTR text = strVal.c_str();
			WritePrivateProfileString(lpAppName, name.c_str(), text, GetFilePath());
		}
		else if (valType == varprop::VT_INT)
		{
			DWORD dwordVal = value.GetInt();
			String strVal = strutils::to_str(dwordVal);
			LPCWSTR text = strVal.c_str();
			WritePrivateProfileString(lpAppName, name.c_str(), text, GetFilePath());
		}
		else if (valType == varprop::VT_BOOL)
		{
			DWORD dwordVal = value.GetBool() ? 1 : 0;
			String strVal = strutils::to_str(dwordVal);
			LPCWSTR text = strVal.c_str();
			WritePrivateProfileString(lpAppName, name.c_str(), text, GetFilePath());
		}
		else
		{
			retVal = COption::OPT_UNKNOWN_TYPE;
		}
	}
	return retVal;
}

/**
 * @brief Set new value for option and save option to file
 */
int CIniOptionsMgr::SaveOption(const String& name, const varprop::VariantValue& value)
{
	int retVal = Set(name, value);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new string value for option and save option to file
 */
int CIniOptionsMgr::SaveOption(const String& name, const String& value)
{
	varprop::VariantValue val;
	val.SetString(value);
	int retVal = Set(name, val);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

/**
 * @brief Set new string value for option and save option to file
 */
int CIniOptionsMgr::SaveOption(const String& name, const TCHAR* value)
{
	return SaveOption(name, String(value));
}

int CIniOptionsMgr::SaveOption(const String& name, int value)
{
	varprop::VariantValue val;
	val.SetInt(value);
	int retVal = Set(name, val);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

int CIniOptionsMgr::SaveOption(const String& name, bool value)
{
	varprop::VariantValue val;
	val.SetBool(value);
	int retVal = Set(name, val);
	if (retVal == COption::OPT_OK)
		retVal = SaveOption(name);
	return retVal;
}

int CIniOptionsMgr::RemoveOption(const String& name)
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

	EnterCriticalSection(&m_cs);

	WritePrivateProfileString(lpAppName, name.c_str(), NULL, GetFilePath());

	LeaveCriticalSection(&m_cs);

	return retVal;
}

String CIniOptionsMgr::ReadValueFromFile(const String& name)
{
	if (m_iniFileKeyValues.empty())
	{
		std::vector<TCHAR> str(32768);
		if (GetPrivateProfileSection(lpAppName, str.data(), static_cast<DWORD>(str.size()), GetFilePath()) > 0)
		{
			TCHAR* p = str.data();
			while (*p)
			{
				TCHAR* v = _tcschr(p, '=');
				if (!v)
					break;
				++v;
				size_t vlen = _tcslen(v);
				m_iniFileKeyValues.insert_or_assign(String{ p, v - 1 }, String{v, v + vlen});
				p = v + vlen + 1;
			}
		}
	}
	return m_iniFileKeyValues[name];
}

int CIniOptionsMgr::ParseValue(const String& strName, String& textValue, varprop::VariantValue& value)
{
	int valType = value.GetType();
	int retVal = COption::OPT_OK;

	if (valType == varprop::VT_STRING)
	{
		value.SetString(textValue);
		retVal = Set(strName, value);
	}
	else if (valType == varprop::VT_INT)
	{
		value.SetInt(std::stoul(textValue));
		retVal = Set(strName, value);
	}
	else if (valType == varprop::VT_BOOL)
	{
		value.SetBool(textValue[0] == '1' ? true : false);
		retVal = Set(strName, value);
	}
	else
		retVal = COption::OPT_WRONG_TYPE;

	return retVal;
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
void CIniOptionsMgr::SplitName(const String& strName, String& strPath,
	String& strValue) const
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