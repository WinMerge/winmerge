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

#include "stdafx.h"

// fix any nonascii characters
// which got sign-extended to become negative integers
int normch(int c)
{
	return (unsigned char)(char)c;
}

int
xisspecial (int c)
{
  return normch(c) > (unsigned) _T ('\x7f') || c == _T ('_');
//  return _tcschr (_T ("Ï?Ë¯?˝·ÌÈÛ˙˘ÔùÚæ‡Â?‰Îˆ¸Ã?»ÿ?›¡Õ…¥O⁄Ÿœç“º¿≈?ƒÀ÷‹ß"), c) != NULL;
}

int
xisalpha (int c)
{
  return _istalpha (normch(c)) || xisspecial (normch(c));
}

int
xisalnum (int c)
{
  return _istalnum (normch(c)) || xisspecial (normch(c));
}

CString LoadResString(int id)
{
	CString s;
	VERIFY(s.LoadString(id));
	return s;
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

// Send message to log (well, to debug window)
void LogErrorString(LPCTSTR sz)
{
	if (!sz || !sz[0]) return;
	CString now = COleDateTime::GetCurrentTime().Format();
	TRACE(_T("%s: %s\n"), (LPCTSTR)now, sz);
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
