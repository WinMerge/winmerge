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
	CRegOptionsMgr();
	virtual ~CRegOptionsMgr();
	int SetRegRootKey(const String& path);
	void CloseKeys();

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

protected:
	HKEY OpenKey(const String& strPath, bool bAlwaysCreate);
	void CloseKey(HKEY hKey, const String& strPath);
	int LoadValueFromBuf(const String& strName, DWORD type, const BYTE* data, varprop::VariantValue &value);
	int LoadValueFromReg(HKEY hKey, const String& strName,
		varprop::VariantValue &value);
	static int SaveValueToReg(HKEY hKey, const String& strValueName,
		const varprop::VariantValue& value);
	static DWORD WINAPI AsyncWriterThreadProc(void *pParam);

private:
	String m_registryRoot; /**< Registry path where to store options. */
	bool m_serializing;
	std::map<String, HKEY> m_hKeys;
	bool m_bCloseHandle;
	DWORD m_dwThreadId;
	HANDLE m_hThread;
	CRITICAL_SECTION m_cs;
	DWORD m_dwQueueCount;
};
