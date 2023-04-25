/**
 * @file IniOptionsMgr.cpp
 *
 * @brief Implementation of Ini file Options management class.
 *
 */

#include "pch.h"
#include "IniOptionsMgr.h"
#include <Windows.h>
#include <process.h>
#include "OptionsMgr.h"

LPCWSTR lpAppName = TEXT("WinMerge");
LPCWSTR lpDefaultSection = TEXT("Defaults");

struct AsyncWriterThreadParams
{
	AsyncWriterThreadParams(const String& name, const varprop::VariantValue& value) : name(name), value(value) {}
	String name;
	varprop::VariantValue value;
};

CIniOptionsMgr::CIniOptionsMgr(const String& filePath)
	: m_serializing(true)
	, m_filePath{filePath}
	, m_dwThreadId(0)
	, m_hThread(nullptr)
	, m_hEvent(nullptr)
	, m_dwQueueCount(0)
{
	m_iniFileKeyValues = Load(m_filePath);
	m_hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	m_hThread = reinterpret_cast<HANDLE>(
		_beginthreadex(nullptr, 0, AsyncWriterThreadProc, this, 0,
			reinterpret_cast<unsigned *>(&m_dwThreadId)));
	WaitForSingleObject(m_hEvent, INFINITE);
	CloseHandle(m_hEvent);
	m_hEvent = nullptr;
}

CIniOptionsMgr::~CIniOptionsMgr()
{
	for (;;) {
		PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
		if (WaitForSingleObject(m_hThread, 1) != WAIT_TIMEOUT)
			break;
	}
}

unsigned __stdcall CIniOptionsMgr::AsyncWriterThreadProc(void *pvThis)
{
	CIniOptionsMgr *pThis = reinterpret_cast<CIniOptionsMgr *>(pvThis);
	MSG msg;
	BOOL bRet;
	// create message queue
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(pThis->m_hEvent);
	while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		auto* pParam = reinterpret_cast<AsyncWriterThreadParams *>(msg.wParam);
		if (msg.message == WM_USER && pParam)
		{
			pThis->SaveValueToFile(pParam->name, pParam->value);
			delete pParam;
			InterlockedDecrement(&pThis->m_dwQueueCount);
		}
	}
	return 0;
}

std::map<String, String> CIniOptionsMgr::Load(const String& iniFilePath)
{
	std::map<String, String> iniFileKeyValues = ReadIniFile(iniFilePath, lpAppName);

	// after reading the "WinMerge" section try to read the "Defaults" section; overwrite existing entries in "iniFileKeyValues" with the ones from the "Defaults" section
	std::map<String, String> iniFileKeyDefaultValues = ReadIniFile(iniFilePath, lpDefaultSection);
	for (auto& [key, strValue] : iniFileKeyDefaultValues)
		iniFileKeyValues.insert_or_assign(key, strValue);
	return iniFileKeyValues;
}

int CIniOptionsMgr::LoadValueFromBuf(const String& strName, const String& textValue, varprop::VariantValue& value)
{
	int retVal = COption::OPT_OK;
	int valType = value.GetType();
	if (valType == varprop::VT_STRING)
	{
		value.SetString(textValue);
		retVal = Set(strName, value);
	}
	else if (valType == varprop::VT_INT)
	{
		tchar_t* endptr = nullptr;
		DWORD val = static_cast<DWORD>(tc::tcstoll(textValue.c_str(), &endptr, 
			(textValue.length() >= 2 && textValue[1] == 'x') ? 16 : 10));
		value.SetInt(static_cast<int>(val));
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

int CIniOptionsMgr::SaveValueToFile(const String& name, const varprop::VariantValue& value)
{
	BOOL retValReg = TRUE;
	int valType = value.GetType();
	int retVal = COption::OPT_OK;

	if (valType == varprop::VT_STRING)
	{
		String strVal = EscapeValue(value.GetString());
		LPCWSTR text = strVal.c_str();
		retValReg = WritePrivateProfileString(lpAppName, name.c_str(), text, GetFilePath());
	}
	else if (valType == varprop::VT_INT)
	{
		DWORD dwordVal = value.GetInt();
		String strVal = strutils::to_str(dwordVal);
		LPCWSTR text = strVal.c_str();
		retValReg = WritePrivateProfileString(lpAppName, name.c_str(), text, GetFilePath());
	}
	else if (valType == varprop::VT_BOOL)
	{
		DWORD dwordVal = value.GetBool() ? 1 : 0;
		String strVal = strutils::to_str(dwordVal);
		LPCWSTR text = strVal.c_str();
		retValReg = WritePrivateProfileString(lpAppName, name.c_str(), text, GetFilePath());
	}
	else if (valType == varprop::VT_NULL)
	{
		auto [strPath, strValueName] = SplitName(name);
		if (!strValueName.empty())
			retValReg = WritePrivateProfileString(lpAppName, name.c_str(), nullptr, GetFilePath());
		else
		{
			auto iniFileMap = Load(GetFilePath());
			for (auto& [key, value2] : iniFileMap)
			{
				if (key.find(strPath) == 0 && key.length() > strPath.length() && key[strPath.length()] == '/')
					retValReg = WritePrivateProfileString(lpAppName, key.c_str(), nullptr, GetFilePath());
			}
		}
	}
	else
	{
		retVal = COption::OPT_UNKNOWN_TYPE;
	}
		
	if (!retValReg)
	{
		retVal = COption::OPT_ERR;
	}
	return retVal;
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

	// Actually save value into our in-memory options table
	int retVal = AddOption(name, defaultValue);

	// Update registry if successfully saved to in-memory table
	if (retVal == COption::OPT_OK)
	{
		// check if value exist
		bool found = m_iniFileKeyValues.find(name) != m_iniFileKeyValues.end();
		if (found)
		{
			String textValue = m_iniFileKeyValues[name];
			varprop::VariantValue value(defaultValue);
			retVal = LoadValueFromBuf(name, textValue, value);
		}
	}

	return retVal;
}

int CIniOptionsMgr::InitOption(const String& name, const String& defaultValue)
{
	varprop::VariantValue defValue;
	defValue.SetString(defaultValue);
	return InitOption(name, defValue);
}

int CIniOptionsMgr::InitOption(const String& name, const tchar_t* defaultValue)
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
		auto* pParam = new AsyncWriterThreadParams(name, value);
		InterlockedIncrement(&m_dwQueueCount);
		if (!PostThreadMessage(m_dwThreadId, WM_USER, (WPARAM)pParam, 0))
			InterlockedDecrement(&m_dwQueueCount);
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
int CIniOptionsMgr::SaveOption(const String& name, const tchar_t* value)
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
	auto [strPath, strValueName] = SplitName(name);

	if (!strValueName.empty())
	{
		retVal = COptionsMgr::RemoveOption(name);
	}
	else
	{
		for (auto it = m_optionsMap.begin(); it != m_optionsMap.end(); )
		{
			const String& key = it->first;
			if (key.find(strPath) == 0 && key.length() > strPath.length() && key[strPath.length()] == '/')
			{
				m_iniFileKeyValues.erase(key);
				it = m_optionsMap.erase(it);
			}
			else
				++it;
		}
		retVal = COption::OPT_OK;
	}

	auto* pParam = new AsyncWriterThreadParams(name, varprop::VariantValue());
	InterlockedIncrement(&m_dwQueueCount);
	if (!PostThreadMessage(m_dwThreadId, WM_USER, (WPARAM)pParam, 0))
		InterlockedDecrement(&m_dwQueueCount);

	return retVal;
}

int CIniOptionsMgr::FlushOptions()
{
	int retVal = COption::OPT_OK;

	while (InterlockedCompareExchange(&m_dwQueueCount, 0, 0) != 0)
		Sleep(0);

	return retVal;
}

int CIniOptionsMgr::ExportOptions(const String& filename, const bool bHexColor /*= false*/) const
{
	for (auto& [key, value] : m_iniFileKeyValues)
	{
		if (m_optionsMap.find(key) == m_optionsMap.end())
		{
			WritePrivateProfileString(_T("WinMerge"), key.c_str(),
				EscapeValue(value).c_str(), filename.c_str());
		}
	}
	return COptionsMgr::ExportOptions(filename, bHexColor);
}

int CIniOptionsMgr::ImportOptions(const String& filename)
{
	int retVal = COptionsMgr::ImportOptions(filename);
	auto iniFileMap = Load(filename);
	for (auto& [key, value] : iniFileMap)
	{
		if (m_optionsMap.find(key) == m_optionsMap.end())
		{
			m_iniFileKeyValues.insert_or_assign(key, value);
			WritePrivateProfileString(_T("WinMerge"), key.c_str(),
				EscapeValue(value).c_str(), GetFilePath());
		}
	}
	return retVal;
}

