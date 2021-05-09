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
	CIniOptionsMgr(const String& filePath);
	virtual ~CIniOptionsMgr();

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

protected:
	static std::map<String, String> Load(const String& iniFilePath);
	int LoadValueFromBuf(const String& strName, String& textValue, varprop::VariantValue& value);
	const TCHAR *GetFilePath() const { return m_filePath.c_str(); }
	int SaveValueToFile(const String& strValueName,
		const varprop::VariantValue& value);
	static DWORD WINAPI AsyncWriterThreadProc(void *pParam);

private:
	bool m_serializing;
	std::map<String, String> m_iniFileKeyValues;
	String m_filePath;
	DWORD m_dwThreadId;
	HANDLE m_hThread;
};
