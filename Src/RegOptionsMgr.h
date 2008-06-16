/** 
 * @file OptionsMgr.h
 *
 * @brief Declaration for Registry options class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$


#ifndef _REG_OPTIONSMGR_H_
#define _REG_OPTIONSMGR_H_

#include "OptionsMgr.h"

class COptionsMgr;

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

#endif // _REG_OPTIONSMGR_H_
