/** 
 * @file OptionsMgr.h
 *
 * @brief Interface for Options management classes
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

/* The MIT License
Copyright (c) 2004 Kimmo Varis
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

#include <afxtempl.h>
#include "varprop.h"

/**
 * @brief Return values for functions
 */
enum
{
	OPT_OK				= 0,
	OPT_ERR				= 1,
	OPT_WRONG_TYPE		= 2,
	OPT_UNKNOWN_TYPE	= 3,
	OPT_NOTFOUND		= 4,
};

/**
 * @brief Class to store option name, value and default value
 */
class COption
{
public:
	int Init(CString name, varprop::VariantValue defaultVal);
	varprop::VariantValue Get() const;
	varprop::VariantValue GetDefault() const;
	int Set(varprop::VariantValue value);
	int SetDefault(varprop::VariantValue defaultValue); 
	void Reset();

private:
	CString m_strName;
	varprop::VariantValue m_value;
	varprop::VariantValue m_valueDef;
};

/**
 * @brief Class to store list of options
 */
class COptionsMgr
{
public:
	int Add(CString name, varprop::VariantValue defaultValue);
	varprop::VariantValue Get(CString name) const;
	CString GetString(CString name) const;
	int GetInt(CString name) const;
	int Set(CString name, varprop::VariantValue value);
	int Reset(CString name);

private:
	CMap<CString, LPCTSTR, COption, COption&> m_optionsMap;
};

/**
 * @brief Class to load/save options to registry
 */
class CRegOptions : public COptionsMgr
{
public:
	int InitOption(CString name, varprop::VariantValue defaultValue);
	int InitOption(CString name, CString defaultValue);
	int InitOption(CString name, int defaultValue);
	int SaveOption(CString name);
	int SaveOption(CString name, varprop::VariantValue value);
	int SaveOption(CString name, CString value);
	int SaveOption(CString name, int value);
	int LoadOption(CString name);
	int SetRegRootKey(CString path);

protected:
	void SplitName(CString strName, CString &strPath, CString &strValue);
	int LoadValueFromReg(HKEY hKey, CString strValueName,
		varprop::VariantValue &value);
	int SaveValueToReg(HKEY hKey, CString strValueName,
		varprop::VariantValue value);

private:
	CString m_registryRoot;

};

#endif // _OPTIONS_MGR_