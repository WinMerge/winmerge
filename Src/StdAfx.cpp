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

// Logging
CLogFile gLog;

/** @brief Report DeleteFile() failure to gLog */
UINT gLog::DeleteFileFailed(LPCTSTR path)
{
	return::gLog.Write(CLogFile::LERROR|CLogFile::LOSERROR|CLogFile::LDEBUG, _T("DeleteFile(%s) failed: "), path);
}

// Convert any negative inputs to negative char equivalents
// This is aimed at correcting any chars mistakenly 
// sign-extended to negative ints.
// This is ok for the UNICODE build because UCS-2LE code bytes
// do not extend as high as 2Gig (actually even full Unicode
// codepoints don't extend that high).
int normch(int c)
{
#ifdef _UNICODE
	return (unsigned short)(short)c;
#else
	return (unsigned char)(char)c;
#endif
}

// Returns nonzero if input is outside ASCII or is underline
int
xisspecial (int c)
{
  return normch(c) > (unsigned) _T ('\x7f') || c == _T ('_');
}

// Returns non-zero if input is alphabetic or "special" (see xisspecial)
// Also converts any negative inputs to negative char equivalents (see normch)
int
xisalpha (int c)
{
  return _istalpha (normch(c)) || xisspecial (normch(c));
}

// Returns non-zero if input is alphanumeric or "special" (see xisspecial)
// Also converts any negative inputs to negative char equivalents (see normch)
int
xisalnum (int c)
{
  return _istalnum (normch(c)) || xisspecial (normch(c));
}

// Returns non-zero if input character is a space
// Also converts any negative inputs to negative char equivalents (see normch)
int
xisspace (int c)
{
  return _istspace (normch(c));
}

// Load string resource and return as CString
CString LoadResString(int id)
{
	CString s;
	VERIFY(s.LoadString(id));
	return s;
}

// Combines AfxFormatString1 with AfxMessageBox
int
ResMsgBox1(int msgid, LPCTSTR arg, UINT nType, UINT nIDHelp)
{
	CString msg;
	AfxFormatString1(msg, msgid, arg);
	if (!nIDHelp) nIDHelp = msgid;
	return AfxMessageBox(msg, nType, nIDHelp);
}

// Get user language description of error, if available
CString GetSysError(int nerr)
{
	LPVOID lpMsgBuf;
	CString str = _T("?");
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
	gLog.Write(CLogFile::LERROR, sz);
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
#ifdef _UNICODE
	return CF_UNICODETEXT;
#else
	return CF_TEXT;
#endif // _UNICODE
}

/**
 * @brief Return true if Unicode (16-bit) build
 */
bool IsUnicodeBuild()
{
#ifdef UNICODE
	return true;
#else
	return false;
#endif
}
