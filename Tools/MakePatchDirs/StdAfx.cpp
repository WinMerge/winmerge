// stdafx.cpp : source file that includes just the standard includes
//	MakePatchDirs.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


CString
MakeStr(LPCTSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	CString str;
	str.FormatV(fmt, args);
	va_end(args);
	return str;
}

CString LoadResString(int id)
{
	CString s;
	VERIFY(s.LoadString(id));
	return s;
}
