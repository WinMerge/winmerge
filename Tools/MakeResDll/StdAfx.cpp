// stdafx.cpp : source file that includes just the standard includes
//	MakeResDll.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
// Following files included from WinMerge/Src/Common
#include "RegKey.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

// Get appropriate clipboard format for TCHAR text
int GetClipTcharTextFormat()
{
#ifdef _UNICODE
	return CF_UNICODETEXT;
#else
	return CF_TEXT;
#endif // _UNICODE
}

// Read-only open of registry key under HKEY_LOCAL_MACHINE
bool
RegOpenMachine(CRegKeyEx & reg, LPCTSTR key)
{
	return reg.OpenNoCreateWithAccess(HKEY_LOCAL_MACHINE, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}

// Read-only open of registry key under HKEY_CURRENT_USER
bool
RegOpenUser(CRegKeyEx & reg, LPCTSTR key)
{
	return reg.OpenNoCreateWithAccess(HKEY_CURRENT_USER, key, KEY_QUERY_VALUE) == ERROR_SUCCESS;
}

