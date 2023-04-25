/** 
 * @file OptionsMgr.cpp
 *
 * @brief Implementation of Options management classes
 *
 */


/* The MIT License
Copyright (c) 2004-2009 Kimmo Varis
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pch.h"
#include "OptionsMgr.h"
#include <algorithm>
#include <cassert>
#include <Windows.h>

constexpr int MAX_PATH_FULL = 32767;

static bool GetAsInt(const String& str, int & val);

varprop::VariantValue COptionsMgr::m_emptyValue;

/**
 * @brief Default constructor.
 */
COption::COption() = default;

/**
 * @brief Copy constructor.
 * @param [in] option Object to copy.
 */
COption::COption(const COption& option)
: m_strName(option.m_strName)
, m_value(option.m_value)
, m_valueDef(option.m_valueDef)
{
}

/**
 * @brief Assignment operator override.
 * @param [in] option Object to copy.
 * @return Copy of given object.
 */
COption& COption::operator=(const COption& option)
{
	if (this != &option)
	{
		m_strName = option.m_strName;
		m_value = option.m_value;
		m_valueDef = option.m_valueDef;
	}
	return *this;
}

/**
 * @brief Set name, value and default value for option
 * @param [in] name Name of option with full path ("Settings/AutomaticRescan")
 * @param [in] defaultValue Default value for option. This value
 * is restored for otion when Reset() is run.
 * @sa COption::Reset()
 */
int COption::Init(const String& name, const varprop::VariantValue& defaultValue)
{
	int retVal = COption::OPT_OK;

	m_strName = name;
	if (m_strName.empty())
		return OPT_ERR;

	// Dont' check type here since we are initing it!
	varprop::VT_TYPE inType = defaultValue.GetType();

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_value.SetBool(defaultValue.GetBool());
		m_valueDef.SetBool(defaultValue.GetBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(defaultValue.GetInt());
		m_valueDef.SetInt(defaultValue.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(defaultValue.GetFloat());
		m_valueDef.SetFloat(defaultValue.GetFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(defaultValue.GetString());
		m_valueDef.SetString(defaultValue.GetString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(defaultValue.GetTime());
		m_valueDef.SetTime(defaultValue.GetTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Convert string to integer.
 * @param [in] str String to convert.
 * @param [out] val Converted integer.
 * @return true if conversion succeeded, false otherwise.
 */
static bool GetAsInt(const String& str, int & val)
{
	if (str.empty())
		return false;
	const size_t len = str.length();

	val = 0;
	for (int i = 0; i < (int)len; ++i)
	{
		int ch = (int)str[i];
		if (ch < '0' || ch > '9')
			return false;
		val *= 10;
		val += ch - '0';
	}
	return true;
}

/**
 * @brief Convert integer value to desired type.
 * @param [in, out] value Value to convert.
 * @param [in] nType Type to convert to.
 * @return true if conversion succeeded, false otherwise.
 * @note Only supports converting to boolean at the moment.
 * @todo Add other conversions (string?).
  */
bool COption::ConvertInteger(varprop::VariantValue & value, varprop::VT_TYPE nType)
{
	int ivalue = value.GetInt();

	switch(nType)
	{
	case varprop::VT_BOOL:
		// Convert integer to boolean
		{
			if (ivalue > 0)
			{
				value.SetBool(true);
				return true;
			}
			else
			{
				value.SetBool(false);
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief Convert string value to desired type.
 * @param [in, out] value Value to convert.
 * @param [in] nType Type to convert to.
 * @return true if conversion succeeded, false otherwise.
 * @note Only supports converting to integer and boolean at the moment.
 * @todo Add other conversions (float?).
  */
bool COption::ConvertString(varprop::VariantValue & value, varprop::VT_TYPE nType)
{
	String svalue = value.GetString();
	switch(nType)
	{
	case varprop::VT_INT:
		// Convert string to integer
		{
			int val=0;
			if (!GetAsInt(svalue, val))
				return false;
			value.SetInt(val);
			return true;
		}
	case varprop::VT_BOOL:
		// Convert string to boolean
		{
			svalue = strutils::makelower(svalue);
			if (svalue == _T("1") || svalue == _T("yes")
				|| svalue == _T("true"))
			{
				value.SetBool(true);
				return true;
			}
			if (svalue == _T("0") || svalue == _T("no")
				|| svalue == _T("false"))
			{
				value.SetBool(false);
				return true;
			}
			return false;
		}
	}
	return false;
}

/**
 * @brief Convert value to desired type.
 * @param [in, out] value Value to convert.
 * @param [in] nType Type to convert to.
 * @return true if conversion succeeded, false otherwise.
 * @note Currently converts only strings and integers.
 * @todo Add other conversions.
 */
bool COption::ConvertType(varprop::VariantValue & value, varprop::VT_TYPE nType)
{
	if (value.GetType() == varprop::VT_STRING)
		return ConvertString(value, nType);
	if (value.GetType() == varprop::VT_INT)
		return ConvertInteger(value, nType);
	return false;
}

/**
 * @brief Set option value.
 * 
 * Set new value for option. If automatic conversion is not allowed,
 * type of value must match to type set when option was initialised.
 * @param [in] value New value to set.
 * @param [in] allowConversion Is automatic type conversion allowed?
 * @sa COption::Init()
 */
int COption::Set(const varprop::VariantValue& value, bool allowConversion)
{
	int retVal = COption::OPT_OK;

	// Check that type matches
	varprop::VT_TYPE inType = value.GetType();
	if (value.GetType() != m_value.GetType())
	{
		if (allowConversion)
		{
			varprop::VariantValue val(value);
			if (ConvertType(val, m_value.GetType()))
				return Set(val);
		}
		return COption::OPT_WRONG_TYPE;
	}

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_value.SetBool(value.GetBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(value.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(value.GetFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(value.GetString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(value.GetTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Change default value for option.
 *
 * Set new value for option default value.  If automatic conversion is not
 * allowed, type of value must match to type set when option was initialised.
 * @param [in] defaultValue New default value.
 * @sa COption::Init()
 */
int COption::SetDefault(const varprop::VariantValue& defaultValue)
{
	int retVal = COption::OPT_OK;

	// Check that type matches
	varprop::VT_TYPE inType = defaultValue.GetType();
	if (inType != m_valueDef.GetType())
	{
		return COption::OPT_WRONG_TYPE;
	}

	switch (inType)
	{
	case varprop::VT_NULL:
		retVal = OPT_UNKNOWN_TYPE;
		break;
	case varprop::VT_BOOL:
		m_valueDef.SetBool(defaultValue.GetBool());
		break;
	case varprop::VT_INT:
		m_valueDef.SetInt(defaultValue.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_valueDef.SetFloat(defaultValue.GetFloat());
		break;
	case varprop::VT_STRING:
		m_valueDef.SetString(defaultValue.GetString());
		break;
	case varprop::VT_TIME:
		m_valueDef.SetTime(defaultValue.GetTime());
		break;
	default:
		retVal = OPT_UNKNOWN_TYPE;
	}
	return retVal;
}

/**
 * @brief Reset option's value to default value.
 */
void COption::Reset()
{
	switch (m_value.GetType())
	{
	case varprop::VT_BOOL:
		m_value.SetBool(m_valueDef.GetBool());
		break;
	case varprop::VT_INT:
		m_value.SetInt(m_valueDef.GetInt());
		break;
	case varprop::VT_FLOAT:
		m_value.SetFloat(m_valueDef.GetFloat());
		break;
	case varprop::VT_STRING:
		m_value.SetString(m_valueDef.GetString());
		break;
	case varprop::VT_TIME:
		m_value.SetTime(m_valueDef.GetTime());
		break;
	}
}

/**
 * @brief Add new option to list.
 * @param [in] name Option's name.
 * @param [in] defaultValue Option's initial and default value.
 */
int COptionsMgr::AddOption(const String& name, const varprop::VariantValue& defaultValue)
{
	COption tmpOption;
	int retVal = tmpOption.Init(name, defaultValue);
	if (retVal == COption::OPT_OK)
		m_optionsMap.insert_or_assign(name, tmpOption);

	return retVal;
}

int COptionsMgr::InitOption(const String& name, int defaultValue, int minValue, int maxValue, bool serializable)
{
	int retVal = InitOption(name, defaultValue, serializable);
	if (retVal == COption::OPT_OK)
	{
		int ival = GetInt(name);
		if (ival < minValue || ival > maxValue)
			Reset(name);
	}
	return retVal;
}

/**
 * @brief Get option value from list by name.
 * @param [in] name Name of the option to get.
 * @return Option's value as variant type.
 */
const varprop::VariantValue& COptionsMgr::Get(const String& name) const
{
	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		return found->second.Get();
	}
	return m_emptyValue;
}

/**
 * @brief Return string option value.
 * @param [in] name Option's name.
 */
const String& COptionsMgr::GetString(const String& name) const
{
	return Get(name).GetString();
}

/**
 * @brief Return integer option value.
 * @param [in] name Option's name.
 */
int COptionsMgr::GetInt(const String& name) const
{
	return Get(name).GetInt();
}

/**
 * @brief Return boolean option value
 * @param [in] name Option's name.
 */
bool COptionsMgr::GetBool(const String& name) const
{
	return Get(name).GetBool();
}

/**
 * @brief Set new value for option.
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::Set(const String& name, const varprop::VariantValue& value)
{
	int retVal = COption::OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		// Allow automatic conversion so we don't bother callsites about this!
		COption tmpOption = found->second;
		retVal = tmpOption.Set(value, true);
		if (retVal == COption::OPT_OK)
			m_optionsMap.insert_or_assign(name, tmpOption);
	}
	else
	{
		retVal = COption::OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Set new value for boolean option.
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::Set(const String& name, bool value)
{
	varprop::VariantValue valx;
	valx.SetBool(value);
	return Set(name, valx);
}

/**
 * @brief Set new value for integer option.
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::Set(const String& name, int value)
{
	varprop::VariantValue valx;
	valx.SetInt(value);
	return Set(name, valx);
}

/**
 * @brief Set new value for string option.
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::Set(const String& name, const String& value)
{
	varprop::VariantValue valx;
	valx.SetString(value);
	return Set(name, valx);
}

/**
 * @brief Set new value for string option.
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::Set(const String& name, const tchar_t *value)
{
	return Set(name, String(value));
}

/**
 * @brief Type-convert and forward to SaveOption(String, int)
 * @param [in] name Option's name.
 * @param [in] value Option's new value.
 */
int COptionsMgr::SaveOption(const String& name, unsigned value)
{
	int xvalue = value;
	return SaveOption(name, xvalue);
}

/**
 * @brief Remove option from options list.
 * @param [in] name Name of the option to remove.
 */
int COptionsMgr::RemoveOption(const String& name)
{
	int retVal = COption::OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		size_t nr_removed = m_optionsMap.erase(name);
		if (nr_removed == 0)
			retVal = COption::OPT_NOTFOUND;
	}
	else
		retVal = COption::OPT_NOTFOUND;

	return retVal;
}

/**
 * @brief Reset option value to default.
 * @param [in] name Name of the option to reset.
 */
int COptionsMgr::Reset(const String& name)
{
	int retVal = COption::OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		COption tmpOption = found->second;
		tmpOption.Reset();
		m_optionsMap.insert_or_assign(name, tmpOption);
	}
	else
	{
		retVal = COption::OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Return default string value.
 * @param [in] name Option's name.
 * @param [out] value Option's default value.
 */
int COptionsMgr::GetDefault(const String& name, String & value) const
{
	int retVal = COption::OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		varprop::VariantValue val = found->second.GetDefault();
		if (val.IsString())
			value = val.GetString();
		else
			retVal = COption::OPT_WRONG_TYPE;
	}
	else
	{
		retVal = COption::OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Return default number value
 * @param [in] name Option's name.
 * @param [out] value Option's default value.
 */
int COptionsMgr::GetDefault(const String& name, unsigned & value) const
{
	int retVal = COption::OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		varprop::VariantValue val = found->second.GetDefault();
		if (val.IsInt())
			value = val.GetInt();
		else
			retVal = COption::OPT_WRONG_TYPE;
	}
	else
	{
		retVal = COption::OPT_NOTFOUND;
	}
	return retVal;
}

/**
 * @brief Return default boolean value
 * @param [in] name Option's name.
 * @param [out] value Option's default value.
 */
int COptionsMgr::GetDefault(const String& name, bool & value) const
{
	int retVal = COption::OPT_OK;

	OptionsMap::const_iterator found = m_optionsMap.find(name);
	if (found != m_optionsMap.end())
	{
		varprop::VariantValue val = found->second.GetDefault();
		if (val.IsBool())
			value = val.GetBool();
		else
			retVal = COption::OPT_WRONG_TYPE;
	}
	else
	{
		retVal = COption::OPT_NOTFOUND;
	}
	return retVal;
}

String COptionsMgr::ExpandShortName(const String& shortname) const
{
	int nmatched = 0;
	String matchedkey;
	for (const auto& it : m_optionsMap)
	{
		if (it.first.find(shortname) != String::npos)
		{
			matchedkey = it.first;
			++nmatched;
		}
	}
	return (nmatched == 1) ? matchedkey : _T("");
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
int COptionsMgr::ExportOptions(const String& filename, const bool bHexColor /*= false*/) const
{
	int retVal = COption::OPT_OK;
	OptionsMap::const_iterator optIter = m_optionsMap.begin();
	while (optIter != m_optionsMap.end() && retVal == COption::OPT_OK)
	{
		const String name(optIter->first);
		String strVal, strType;
		varprop::VariantValue value = optIter->second.Get();
		if (value.GetType() == varprop::VT_BOOL)
		{
			strType = _T("bool");
			if (value.GetBool())
				strVal = _T("1");
			else
				strVal = _T("0");
		}
		else if (value.GetType() == varprop::VT_INT)
		{
			strType = _T("int");
			if ( bHexColor && (strutils::makelower(name).find(String(_T("color"))) != std::string::npos) )
				strVal = strutils::format(_T("0x%06x"), value.GetInt());
			else
				strVal = strutils::to_str(value.GetInt());
		}
		else if (value.GetType() == varprop::VT_STRING)
		{
			strType = _T("string");
			strVal = EscapeValue(value.GetString());
		}

		bool bRet = !!WritePrivateProfileString(_T("WinMerge"), name.c_str(),
				strVal.c_str(), filename.c_str());
		if (!bRet)
			retVal = COption::OPT_ERR;
		bRet = !!WritePrivateProfileString(_T("WinMerge.TypeInfo"), name.c_str(),
				strType.c_str(), filename.c_str());
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
int COptionsMgr::ImportOptions(const String& filename)
{
	int retVal = COption::OPT_OK;
	auto oleTranslateColor = [](unsigned color) -> unsigned { return ((color & 0xffffff00) == 0x80000000) ? GetSysColor(color & 0x000000ff) : color; };

	// Query keys - returns NUL separated strings
	auto iniFileKeyValues = ReadIniFile(filename, _T("WinMerge"));
	if (iniFileKeyValues.empty())
		return COption::OPT_NOTFOUND;

	for (auto& [key, strValue] : iniFileKeyValues)
	{
		varprop::VariantValue value = Get(key);

		if (value.GetType() == varprop::VT_BOOL)
		{
			value.SetBool(tc::ttoi(strValue.c_str()) != 0);
		}
		else if (value.GetType() == varprop::VT_INT)
		{
			tchar_t* endptr = nullptr;
			unsigned uval = static_cast<unsigned>(tc::tcstoll(strValue.c_str(), &endptr,
				(strValue.length() >= 2 && strValue[1] == 'x') ? 16 : 10));
			int intVal = static_cast<int>(uval);
			if (strutils::makelower(key).find(String(_T("color"))) != std::string::npos)
				intVal = static_cast<int>(oleTranslateColor(static_cast<unsigned>(intVal)));
			value.SetInt(intVal);
		}
		else if (value.GetType() == varprop::VT_STRING)
		{
			value.SetString(strValue);
		}

		if (value.GetType() != varprop::VT_NULL)
			SaveOption(key, value);
	}
	FlushOptions();
	return retVal;
}

String COptionsMgr::EscapeValue(const String& text)
{
	String text2;
	for (size_t i = 0; i < text.length(); ++i)
	{
		tchar_t ch = text[i];
		if (ch == '\0' || ch == '\x1b' || ch == '\r' || ch == '\n')
		{
			text2 += '\x1b';
			text2 += ch + '@';
		}
		else
			text2 += ch;
	}
	return text2;
}

String COptionsMgr::UnescapeValue(const String& text)
{
	if (text.find('\x1b') == String::npos)
		return text;
	String text2;
	for (size_t i = 0; i < text.length(); ++i)
	{
		if (text[i] == '\x1b' && i < text.length() - 1)
		{
			++i;
			text2 += text[i] - '@';
		}
		else
			text2 += text[i];
	}
	return text2;
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
std::pair<String, String> COptionsMgr::SplitName(const String& strName)
{
	String strValue, strPath;
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
	return { strPath, strValue };
}

std::map<String, String> COptionsMgr::ReadIniFile(const String& filename, const String& section)
{
	std::map<String, String> iniFileKeyValues;
	std::vector<tchar_t> str(65536);
	if (GetPrivateProfileSection(section.c_str(), str.data(), static_cast<DWORD>(str.size()), filename.c_str()) > 0)
	{
		const tchar_t* p = str.data();
		while (*p)
		{
			const tchar_t* v = tc::tcschr(p, '=');
			if (!v)
				break;
			++v;
			size_t vlen = tc::tcslen(v);
			String value{ v, v + vlen };
			String key{ p, v - 1 };
			iniFileKeyValues.insert_or_assign(key, UnescapeValue(value));
			p = v + vlen + 1;
		}
	}
	return iniFileKeyValues;
}

