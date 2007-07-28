/** 
 * @file OptionsMgr.h
 *
 * @brief Interface for Options management classes
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

/* The MIT License
Copyright (c) 2004-2007 Kimmo Varis
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


#ifndef _OPTIONS_MGR_
#define _OPTIONS_MGR_

#include <map>
#include "UnicodeString.h"
#include "varprop.h"


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

/**
 * @brief Class to store option name, value and default value.
 */
class COption
{
public:
	COption();
	COption(const COption& option);

	COption& operator=(const COption& option);

	int Init(LPCTSTR name, varprop::VariantValue defaultVal);
	varprop::VariantValue Get() const;
	varprop::VariantValue GetDefault() const;
	int Set(varprop::VariantValue value, bool allowConversion = false);
	int SetDefault(varprop::VariantValue defaultValue); 
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
	int AddOption(LPCTSTR name, varprop::VariantValue defaultValue);
	varprop::VariantValue Get(LPCTSTR name) const;
	String GetString(LPCTSTR name) const;
	int GetInt(LPCTSTR name) const;
	void SetInt(LPCTSTR name, int value) { SaveOption(name, value); }
	bool GetBool(LPCTSTR name) const;
	void SetBool(LPCTSTR name, bool value) { SaveOption(name, value); }
	int Set(LPCTSTR name, varprop::VariantValue value);
	int Reset(LPCTSTR name);
	int GetDefault(LPCTSTR name, String & value) const;
	int GetDefault(LPCTSTR name, DWORD & value) const;
	int GetDefault(LPCTSTR name, bool & value) const;

	virtual int InitOption(LPCTSTR name,
		varprop::VariantValue defaultValue) = 0;
	virtual int InitOption(LPCTSTR name, LPCTSTR defaultValue) = 0;
	virtual int InitOption(LPCTSTR name, int defaultValue, bool serializable = true) = 0;
	virtual int InitOption(LPCTSTR name, bool defaultValue) = 0;

	virtual int SaveOption(LPCTSTR name) = 0;
	virtual int SaveOption(LPCTSTR name, varprop::VariantValue value) = 0;
	virtual int SaveOption(LPCTSTR name, LPCTSTR value) = 0;
	virtual int SaveOption(LPCTSTR name, int value) = 0;
	virtual int SaveOption(LPCTSTR name, bool value) = 0;
	virtual int SaveOption(LPCTSTR name, UINT value);
	virtual int SaveOption(LPCTSTR name, COLORREF value);

	virtual int RemoveOption(LPCTSTR name);

	virtual int ExportOptions(LPCTSTR filename);
	virtual int ImportOptions(LPCTSTR filename);
	
	virtual void SetSerializing(bool serializing=true) = 0;

private:
	OptionsMap m_optionsMap; /**< Map where options are stored. */
};

/**
 * @brief Registry-based implementation of OptionsMgr interface (q.v.).
 */
class CRegOptionsMgr: public COptionsMgr
{
public:
	CRegOptionsMgr() : m_serializing(true) { }

	int LoadOption(LPCTSTR name);
	int SetRegRootKey(LPCTSTR path);

	virtual int InitOption(LPCTSTR name, varprop::VariantValue defaultValue);
	virtual int InitOption(LPCTSTR name, LPCTSTR defaultValue);
	virtual int InitOption(LPCTSTR name, int defaultValue, bool serializable=true);
	virtual int InitOption(LPCTSTR name, bool defaultValue);

	virtual int SaveOption(LPCTSTR name);
	virtual int SaveOption(LPCTSTR name, varprop::VariantValue value);
	virtual int SaveOption(LPCTSTR name, LPCTSTR value);
	virtual int SaveOption(LPCTSTR name, int value);
	virtual int SaveOption(LPCTSTR name, bool value);

	virtual int RemoveOption(LPCTSTR name);

	virtual void SetSerializing(bool serializing=true) { m_serializing = serializing; }

protected:
	void SplitName(String strName, String &strPath, String &strValue);
	int LoadValueFromReg(HKEY hKey, LPCTSTR strName,
		varprop::VariantValue &value);
	int SaveValueToReg(HKEY hKey, LPCTSTR strValueName,
		varprop::VariantValue value);

private:
	String m_registryRoot; /**< Registry path where to store options. */
	bool m_serializing;

};

#endif // _OPTIONS_MGR_