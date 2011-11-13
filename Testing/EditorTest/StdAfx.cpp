// stdafx.cpp : source file that includes just the standard includes
//	Sample.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


/**
 * @brief Load string resource and return as CString.
 * @param [in] id Resource string ID.
 * @return Resource string as CString.
 */
String LoadResString(UINT id)
{
	TCHAR szBuf[512];
	LoadString(NULL, id, szBuf, sizeof(szBuf));
	return String(szBuf);
}

/**
 * @brief Wrapper around CMergeApp::TranslateDialog()
 */
void NTAPI LangTranslateDialog(HWND h)
{
}

/**
 * @brief Lang aware version of AfxFormatStrings()
 */
void NTAPI LangFormatStrings(CString &rString, UINT id, LPCTSTR const *rglpsz, int nString)
{
	String fmt = LoadResString(id);
	AfxFormatStrings(rString, fmt.c_str(), rglpsz, nString);
}

/**
 * @brief Lang aware version of AfxFormatString1()
 */
void NTAPI LangFormatString1(CString &rString, UINT id, LPCTSTR lpsz1)
{
	LangFormatStrings(rString, id, &lpsz1, 1);
}

// Get user language description of error, if available
String GetSysError(int nerr)
{
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

// Make a CString from printf-style args (single call version of CString::Format)
CString Fmt(LPCTSTR fmt, ...)
{
	CString str;
	va_list args;
	va_start(args, fmt);
	str.FormatV(fmt, args);
	va_end(args);
	return str;
}

/**
 * @brief Get appropriate default codepage 
 */
int getDefaultCodepage()
{
	return CP_ACP;
}

