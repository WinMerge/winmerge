#include "StdAfx.h"
#include "MergeApp.h"
#include "Merge.h"

// Get user language description of error, if available
String GetSysError(int nerr /* =-1 */)
{
	if (nerr == -1)
		nerr = GetLastError();
	LPVOID lpMsgBuf;
	String str = _T("?");
	if (FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		nerr,
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		))
	{
		str = (LPCTSTR)lpMsgBuf;
	}
	// Free the buffer.
	LocalFree( lpMsgBuf );
	return str;
}

/**
 * @brief Get Options Manager.
 * @return Pointer to OptionsMgr.
 */
COptionsMgr * GetOptionsMgr()
{
	CMergeApp *pApp = static_cast<CMergeApp *>(AfxGetApp());
	return pApp->GetMergeOptionsMgr();
}

// Send message to log and debug window
void LogErrorString(const String& sz)
{
	if (sz.empty()) return;
	CString now = COleDateTime::GetCurrentTime().Format();
	TRACE(_T("%s: %s\n"), (LPCTSTR)now, sz.c_str());
}

// Send message to log and debug window
void LogErrorStringUTF8(const std::string& sz)
{
	if (sz.empty()) return;
	String str = ucr::toTString(sz);
	CString now = COleDateTime::GetCurrentTime().Format();
	TRACE(_T("%s: %s\n"), (LPCTSTR)now, str.c_str());
}

/**
 * @brief Load string resource and return as CString.
 * @param [in] id Resource string ID.
 * @return Resource string as CString.
 */
String LoadResString(unsigned id)
{
	return theApp.LoadString(id);
}

String tr(const std::string &str)
{
	String translated_str;
	theApp.TranslateString(str, translated_str);
	return translated_str;
}

void AppErrorMessageBox(const String& msg)
{
	AfxMessageBox(msg.c_str(), MB_ICONSTOP);
}
