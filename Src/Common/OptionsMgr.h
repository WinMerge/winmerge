/** 
 * @file OptionsMgr.h
 *
 * @brief Interface for Options management classes
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

#pragma once

#include <map>
#include <vector>
#include "UnicodeString.h"
#include "varprop.h"

/**
 * @brief Class to store option name, value and default value.
 */
class COption
{
public:
	/**
	* @brief Return values for functions.
	*/
	enum
	{
		OPT_OK				= 0, /**< All good. */
		OPT_ERR				= 1, /**< General error. */
		OPT_WRONG_TYPE		= 2, /**< Option type was wrong. */
		OPT_UNKNOWN_TYPE	= 3, /**< Given option type is not known. */
		OPT_NOTFOUND		= 4, /**< Option name not found. */
	};

	COption();
	COption(const COption& option);

	COption& operator=(const COption& option);

	int Init(const String& name, const varprop::VariantValue& defaultVal);
	const varprop::VariantValue& Get() const;
	const varprop::VariantValue& GetDefault() const;
	int Set(const varprop::VariantValue& value, bool allowConversion = false);
	int SetDefault(const varprop::VariantValue& defaultValue); 
	void Reset();

protected:
	bool ConvertInteger(varprop::VariantValue & value, varprop::VT_TYPE nType);
	bool ConvertString(varprop::VariantValue & value, varprop::VT_TYPE nType);
	bool ConvertType(varprop::VariantValue & value, varprop::VT_TYPE nType);

private:
	String m_strName; /**< Option's name. */
	varprop::VariantValue m_value; /**< Option's current value. */
	varprop::VariantValue m_valueDef; /**< Option's default value. */
};

/**
 * @brief Return option value.
 * @return Value as Variant type.
 */
inline const varprop::VariantValue& COption::Get() const
{
	return m_value;
}

/**
 * @brief Return option default value.
 * @return Default value as varian type.
 */
inline const varprop::VariantValue& COption::GetDefault() const
{
	return m_valueDef;
}

typedef std::map<String, COption> OptionsMap;

/**
 * @brief Class to store list of options.
 * This class holds a list of all options (known to application). Options
 * are accessed by their name.
 *
 * Option must be first initialized before it can be read/set. Initialization
 * is done with InitOption() method.
 */
class COptionsMgr
{
public:
	virtual ~COptionsMgr() {}
	int AddOption(const String& name, const varprop::VariantValue& defaultValue);
	const varprop::VariantValue& Get(const String& name) const;
	const String& GetString(const String& name) const;
	int GetInt(const String& name) const;
	bool GetBool(const String& name) const;
	int Set(const String& name, const varprop::VariantValue& value);
	int Set(const String& name, const String& value);
	int Set(const String& name, const tchar_t *value);
	int Set(const String& name, bool value);
	int Set(const String& name, int value);
	int Reset(const String& name);
	int GetDefault(const String& name, String & value) const;
	int GetDefault(const String& name, unsigned & value) const;
	int GetDefault(const String& name, bool & value) const;
	template <typename T> T GetDefault(const String& name) const { T v; GetDefault(name, v); return v; }
	template <template<typename T, typename = std::allocator<T>> class Container = std::vector>
	Container<String> GetNameList() const
	{
		Container<String> nameList;
		for (const auto& e : m_optionsMap)
			nameList.push_back(e.first);
		return nameList;
	}
	String ExpandShortName(const String & shortname) const;

	virtual int InitOption(const String& name, const varprop::VariantValue& defaultValue) = 0;
	virtual int InitOption(const String& name, const String& defaultValue) = 0;
	virtual int InitOption(const String& name, const tchar_t *defaultValue) = 0;
	virtual int InitOption(const String& name, int defaultValue, bool serializable = true) = 0;
	virtual int InitOption(const String& name, int defaultValue, int minValue, int maxValue, bool serializable = true);
	virtual int InitOption(const String& name, bool defaultValue) = 0;

	virtual int SaveOption(const String& name) = 0;
	virtual int SaveOption(const String& name, const varprop::VariantValue& value) = 0;
	virtual int SaveOption(const String& name, const String& value) = 0;
	virtual int SaveOption(const String& name, const tchar_t *value) = 0;
	virtual int SaveOption(const String& name, int value) = 0;
	virtual int SaveOption(const String& name, bool value) = 0;
	virtual int SaveOption(const String& name, unsigned value);

	virtual int RemoveOption(const String& name);

	virtual int FlushOptions() = 0;

	virtual int ExportOptions(const String& filename, const bool bHexColor=false) const;
	virtual int ImportOptions(const String& filename);
	
	virtual void SetSerializing(bool serializing=true) = 0;

protected:
	static String EscapeValue(const String& text);
	static String UnescapeValue(const String& text);
	static std::pair<String, String> SplitName(const String& strName);
	static std::map<String, String> ReadIniFile(const String& filename, const String& section);

	OptionsMap m_optionsMap; /**< Map where options are stored. */

private:
	static varprop::VariantValue m_emptyValue;
};
