/** 
 * @file OptionsMgr.h
 *
 * @brief Declaration for Registry options class.
 *
 */
#pragma once

#include <Windows.h>
#include "OptionsMgr.h"

class COptionsMgr;

/**
 * @brief Registry-based implementation of OptionsMgr interface (q.v.).
 */
class CRegOptionsMgr: public COptionsMgr
{
public:
	CRegOptionsMgr() : m_serializing(true), m_bCloseHandle(false) { }
	virtual ~CRegOptionsMgr() { }
	int LoadOption(const String& name);
	int SetRegRootKey(const String& path);
	void CloseHandles();

	virtual int InitOption(const String& name, const varprop::VariantValue& defaultValue) override;
	virtual int InitOption(const String& name, const String& defaultValue) override;
	virtual int InitOption(const String& name, const TCHAR *defaultValue) override;
	virtual int InitOption(const String& name, int defaultValue, bool serializable=true) override;
	virtual int InitOption(const String& name, bool defaultValue) override;

	virtual int SaveOption(const String& name) override;
	virtual int SaveOption(const String& name, const varprop::VariantValue& value) override;
	virtual int SaveOption(const String& name, const String& value) override;
	virtual int SaveOption(const String& name, const TCHAR *value) override;
	virtual int SaveOption(const String& name, int value) override;
	virtual int SaveOption(const String& name, bool value) override;

	virtual int RemoveOption(const String& name) override;

	virtual void SetSerializing(bool serializing=true) override { m_serializing = serializing; }

	virtual int ExportOptions(const String& filename, const bool bHexColor=false) const override;
	virtual int ImportOptions(const String& filename) override;

protected:
	void SplitName(const String &strName, String &strPath, String &strValue) const;
	int LoadValueFromReg(HKEY hKey, const String& strName,
		varprop::VariantValue &value);
	int SaveValueToReg(HKEY hKey, const String& strValueName,
		const varprop::VariantValue& value);

private:
	String m_registryRoot; /**< Registry path where to store options. */
	bool m_serializing;
	std::map<String, HKEY> m_hKeys;
	bool m_bCloseHandle;
};
