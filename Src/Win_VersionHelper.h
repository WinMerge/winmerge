/** 
 * @file  Win_VersionHelper.h
 *
 * @brief Defines quick-and-dirty runtime Windows version checking.

 * All we need in WinMerge is `IsWin7_OrGreater()`
 * But other possibilities (and slightly different implementation) can be seen in ...
 *		'C:\Program Files (x86)\Windows Kits\10\Include\10.0.17134.0\um\VersionHelpers.h'
 *
 */

#pragma once

#include <windows.h>

inline bool
IsWinVer_OrGreater(WORD wVersion, WORD wServicePack = 0)
{
	DWORDLONG dwlConditionMask = 0;	
	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL );

	OSVERSIONINFOEXW osvi;
	::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = HIBYTE(wVersion);
	osvi.dwMinorVersion = LOBYTE(wVersion);
	osvi.wServicePackMajor = wServicePack;

	return !!VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask);
}


inline bool
IsWin7_OrGreater()
{
    return IsWinVer_OrGreater( _WIN32_WINNT_WIN7 );
}

inline bool
IsVista_OrGreater()
{
    return IsWinVer_OrGreater( _WIN32_WINNT_VISTA );
}
