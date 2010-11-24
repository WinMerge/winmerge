/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// stdafx.cpp : source file that includes just the standard includes
//	Merge.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information
//
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "LogFile.h"
#include "Merge.h"


/**
 * @brief Load string resource and return as CString.
 * @param [in] id Resource string ID.
 * @return Resource string as CString.
 */
String LoadResString(UINT id)
{
	return theApp.LoadString(id);
}

/**
 * @brief Wrapper around CMergeApp::TranslateDialog()
 */
void NTAPI LangTranslateDialog(HWND h)
{
	theApp.TranslateDialog(h);
}

/**
 * @brief Lang aware version of AfxFormatStrings()
 */
void NTAPI LangFormatStrings(CString &rString, UINT id, LPCTSTR const *rglpsz, int nString)
{
	String fmt = theApp.LoadString(id);
	AfxFormatStrings(rString, fmt.c_str(), rglpsz, nString);
}

/**
 * @brief Lang aware version of AfxFormatString1()
 */
void NTAPI LangFormatString1(CString &rString, UINT id, LPCTSTR lpsz1)
{
	LangFormatStrings(rString, id, &lpsz1, 1);
}

/**
 * @brief Lang aware version of AfxFormatString2()
 */
void NTAPI LangFormatString2(CString &rString, UINT id, LPCTSTR lpsz1, LPCTSTR lpsz2)
{
	LPCTSTR rglpsz[2] = { lpsz1, lpsz2 };
	LangFormatStrings(rString, id, rglpsz, 2);
}

/**
 * @brief Lang aware version of AfxMessageBox()
 */
int NTAPI LangMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp)
{
	String string = theApp.LoadString(nIDPrompt);
	if (nIDHelp == (UINT)-1)
		nIDHelp = nIDPrompt;
	return AfxMessageBox(string.c_str(), nType, nIDHelp);
}

/**
 * @brief Show messagebox with resource string having parameter.
 * @param [in] msgid Resource string ID.
 * @param [in] arg Argument string.
 * @param [in] nType Messagebox type flags (e.g. MB_OK).
 * @param [in] nIDHelp Help string ID.
 * @return User choice from the messagebox (see MessageBox()).
 */
int ResMsgBox1(UINT msgid, LPCTSTR arg, UINT nType, UINT nIDHelp)
{
	CString msg;
	LangFormatString1(msg, msgid, arg);
	if (!nIDHelp)
		nIDHelp = msgid;
	return AfxMessageBox(msg, nType, nIDHelp);
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

// Send message to log and debug window
void LogErrorString(LPCTSTR sz)
{
	if (!sz || !sz[0]) return;
	CString now = COleDateTime::GetCurrentTime().Format();
	TRACE(_T("%s: %s\n"), (LPCTSTR)now, sz);

#if defined (_DEBUG) || defined (ENABLE_LOG)
	GetLog()->Write(CLogFile::LERROR, sz);
#endif
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

// Get appropriate clipboard format for TCHAR text
int GetClipTcharTextFormat()
{
	return CF_UNICODETEXT;
}

#if _MSC_VER <= 1310
/**
* @brief Calculates a number absolute value.
*/
__int64 _abs64_patch(__int64 n)
{
	return ((n >= 0) ? n : -n);
}
#endif
