/**
 * @file IniOptionsMgr.h
 *
 * @brief Implementation of Ini file Options management class.
 *
 */
#pragma once

#include <Windows.h>
#include "OptionsMgr.h"

class COptionsMgr;

/**
 * @brief Ini-based implementation of OptionsMgr interface (q.v.).
 */
class CIniOptionsMgr : public COptionsMgr
{
public:
	CIniOptionsMgr();
	virtual ~CIniOptionsMgr();

	static bool CheckIfIniFileExist();

	static LPCWSTR GetFilePath();

	virtual int InitOption(const String& name, const varprop::VariantValue& defaultValue) override;
	virtual int InitOption(const String& name, const String& defaultValue) override;
	virtual int InitOption(const String& name, const TCHAR* defaultValue) override;
	virtual int InitOption(const String& name, int defaultValue, bool serializable = true) override;
	virtual int InitOption(const String& name, bool defaultValue) override;

	virtual int SaveOption(const String& name) override;
	virtual int SaveOption(const String& name, const varprop::VariantValue& value) override;
	virtual int SaveOption(const String& name, const String& value) override;
	virtual int SaveOption(const String& name, const TCHAR* value) override;
	virtual int SaveOption(const String& name, int value) override;
	virtual int SaveOption(const String& name, bool value) override;

	virtual int RemoveOption(const String& name) override;

	virtual void SetSerializing(bool serializing = true) override { m_serializing = serializing; }

private:
	CRITICAL_SECTION m_cs;
	bool m_serializing;
	static LPCWSTR lpFilePath;
	std::map<String, String> m_iniFileKeyValues;

	String ReadValueFromFile(const String& name);
	int ParseValue(const String& strName, String& textValue, varprop::VariantValue& value);

	void SplitName(const String& strName, String& strPath, String& strValue) const;
};