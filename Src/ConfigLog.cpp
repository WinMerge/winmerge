/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ConfigLog.cpp
 *
 * @brief CConfigLog implementation
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#ifndef UNICODE
#include <mbctype.h>
#endif
#include "version.h"
#include "dllver.h"
#include "DiffWrapper.h"
#include "ConfigLog.h"
#include "winnt_supp.h"
#include "Plugins.h"
#include "paths.h"
#include "unicoder.h"
#include "codepage.h"

/** 
 * @brief Return logfile name and path
 */
CString CConfigLog::GetFileName() const
{
	return m_sFileName;
}

/** 
 * @brief Write plugin names
 */
void CConfigLog::WritePluginsInLogFile(LPCWSTR transformationEvent, CStdioFile & file)
{
	// get an array with the available scripts
	PluginArray * piPluginArray; 

	piPluginArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(transformationEvent);

	int iPlugin;
	for (iPlugin = 0 ; iPlugin < piPluginArray->GetSize() ; iPlugin++)
	{
		PluginInfo & plugin = piPluginArray->ElementAt(iPlugin);
		file.WriteString(_T("\n  "));
		file.WriteString(plugin.name);
	}
}

/**
 * @brief CString wrapper around API call GetLocaleInfo
 */
static CString GetLocaleString(LCID locid, LCTYPE lctype)
{
	TCHAR buffer[512];
	if (!GetLocaleInfo(locid, lctype, buffer, sizeof(buffer)/sizeof(buffer[0])))
		buffer[0] = 0;
	return buffer;
}

/**
 * @brief Write out various possibly relevant windows locale information
 */
static void WriteLocaleSettings(CStdioFile & file, LCID locid, LPCTSTR title)
{
	file.WriteString(Fmt(_T(" %s:\n"), title));
	file.WriteString(Fmt(_T("  Def ANSI codepage: %s\n"), GetLocaleString(locid, LOCALE_IDEFAULTANSICODEPAGE)));
	file.WriteString(Fmt(_T("  Def OEM codepage: %s\n"), GetLocaleString(locid, LOCALE_IDEFAULTCODEPAGE)));
	file.WriteString(Fmt(_T("  Country: %s\n"), GetLocaleString(locid, LOCALE_SENGCOUNTRY)));
	file.WriteString(Fmt(_T("  Language: %s\n"), GetLocaleString(locid, LOCALE_SENGLANGUAGE)));
	file.WriteString(Fmt(_T("  Language code: %s\n"), GetLocaleString(locid, LOCALE_ILANGUAGE)));
	file.WriteString(Fmt(_T("  ISO Language code: %s\n"), GetLocaleString(locid, LOCALE_SISO639LANGNAME)));
}

/** 
 * @brief Write logfile
 */
BOOL CConfigLog::WriteLogFile(CString &sError)
{
	CStdioFile file;
	CFileException e;
	CVersionInfo version;
	CString tempPath;
	CString text;

	m_sFileName = _T("WinMerge.txt");

	// Get path to $temp/WinMerge.txt
	if (GetTempPath(MAX_PATH, tempPath.GetBuffer(MAX_PATH)))
	{
		tempPath.ReleaseBuffer();
		m_sFileName.Insert(-1, tempPath);
	}
	else
		return FALSE;

	// Convert from 8.3 style as yielded by GetTempPath to normal style
	m_sFileName = paths_GetLongPath(m_sFileName, NODIRSLASH);

	if (!file.Open(m_sFileName, CFile::modeCreate | CFile::modeWrite))
	{
		TCHAR szError[1024];
		e.GetErrorMessage(szError, 1024);
		sError = szError;
		return FALSE;
	}

// Begin log
	file.WriteString(_T("WinMerge configuration log\n"));
	file.WriteString(_T("--------------------------\n"));
	file.WriteString(_T("Saved to: "));
	file.WriteString(m_sFileName);
	file.WriteString(_T("\n* Please add this information (or attach this file)\n"));
	file.WriteString(_T("* when reporting bugs.\n"));

// Platform stuff
	file.WriteString(_T("\n\nVersion information:\n"));
	file.WriteString(_T(" WinMerge.exe: "));
	file.WriteString(version.GetFixedProductVersion());

	CString privBuild = version.GetPrivateBuild();
	if (!privBuild.IsEmpty())
	{
		file.WriteString(_T(" - Private build: "));
		file.WriteString(privBuild);
	}

	text = GetBuildFlags();
	file.WriteString(_T("\n Build config: "));
	file.WriteString(text);

	file.WriteString(_T("\n Windows: "));
	text = GetWindowsVer();
	file.WriteString(text);
		
	DWORD dllVersion = GetDllVersion(_T("comctl32.dll"));;
	text.Format(_T("\n COMCTL32.dll: %u.%u\n"),
		HIWORD(dllVersion), LOWORD(dllVersion));
	file.WriteString(text);
	
	dllVersion = GetDllVersion(_T("ShellExtension.dll"));;
	if (dllVersion > 0)
	{
		text.Format(_T("\n ShellExtension.dll: %u.%u\n"),
			HIWORD(dllVersion), LOWORD(dllVersion));
		file.WriteString(text);
	}

// WinMerge settings
	file.WriteString(_T("\nWinMerge configuration:\n"));
	file.WriteString(_T(" Compare settings:\n"));
	file.WriteString(_T("  Ignore blank lines: "));
	if (m_diffOptions.bIgnoreBlankLines)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Ignore case: "));
	if (m_diffOptions.bIgnoreCase)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Ignore carriage return differences: "));
	if (m_diffOptions.bEolSensitive)
		file.WriteString(_T("No\n"));
	else
		file.WriteString(_T("Yes\n"));

	file.WriteString(_T("  Whitespace compare: "));
	switch (m_diffOptions.nIgnoreWhitespace)
	{
	case WHITESPACE_COMPARE_ALL:
		file.WriteString(_T("Compare all\n"));
		break;
	case WHITESPACE_IGNORE_CHANGE:
		file.WriteString(_T("Ignore change\n"));
		break;
	case WHITESPACE_IGNORE_ALL:
		file.WriteString(_T("Ignore all\n"));
		break;
	default:
		file.WriteString(_T("Unknown\n"));
		break;
	}

	file.WriteString(_T("  Detect moved blocks: "));
	if (m_miscSettings.bMovedBlocks)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("\n Other settings:\n"));
	file.WriteString(_T("  Automatic rescan: "));
	if (m_miscSettings.bAutomaticRescan)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Simple EOL: "));
	if (m_miscSettings.bAllowMixedEol)
		file.WriteString(_T("No\n"));
	else
		file.WriteString(_T("Yes\n"));

	file.WriteString(_T("  Automatic scroll to 1st difference: "));
	if (m_miscSettings.bScrollToFirst)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Backup original file: "));
	if (m_miscSettings.bBackup)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("\n Show:\n"));
	file.WriteString(_T("  Identical files: "));
	if (m_viewSettings.bShowIdent)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Different files: "));
	if (m_viewSettings.bShowDiff)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Left Unique files: "));
	if (m_viewSettings.bShowUniqueLeft)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Right Unique files: "));
	if (m_viewSettings.bShowUniqueRight)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Binary files: "));
	if (m_viewSettings.bShowBinaries)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Skipped files: "));
	if (m_viewSettings.bShowSkipped)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  *.bak files: "));
	if (m_viewSettings.bHideBak)
		file.WriteString(_T("No\n"));
	else
		file.WriteString(_T("Yes\n"));

	file.WriteString(_T("\n View Whitespace: "));
	if (m_miscSettings.bViewWhitespace)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

// System settings
	file.WriteString(_T("\nSystem settings:\n"));
	file.WriteString(_T(" codepage settings:\n"));
	file.WriteString(Fmt(_T("  ANSI codepage: %d\n"), GetACP()));
	file.WriteString(Fmt(_T("  OEM codepage: %d\n"), GetOEMCP()));
#ifndef UNICODE
	file.WriteString(Fmt(_T("  multibyte codepage: %d\n"), _getmbcp()));
#endif
	WriteLocaleSettings(file, GetThreadLocale(), _T("Locale (Thread)"));
	WriteLocaleSettings(file, LOCALE_USER_DEFAULT, _T("Locale (User)"));
	WriteLocaleSettings(file, LOCALE_SYSTEM_DEFAULT, _T("Locale (System)"));
//	file.WriteString(Fmt(_T(" unicoder codepage: %d\n"), getDefaultCodepage()));

// Codepage settings
	file.WriteString(_T("Detect codepage automatically for RC and HTML files: "));
	if (m_cpSettings.bDetectCodepage)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));
	file.WriteString(Fmt(_T(" unicoder codepage: %d\n"), getDefaultCodepage()));

// Plugins
	file.WriteString(_T("\nPlugins: "));
	file.WriteString(_T("\n Unpackers: "));
	WritePluginsInLogFile(L"FILE_PACK_UNPACK", file);
	WritePluginsInLogFile(L"BUFFER_PACK_UNPACK", file);
	file.WriteString(_T("\n Prediffers: "));
	WritePluginsInLogFile(L"FILE_PREDIFF", file);
	WritePluginsInLogFile(L"BUFFER_PREDIFF", file);
	file.WriteString(_T("\n Editor scripts: "));
	WritePluginsInLogFile(L"EDITOR_SCRIPT", file);
	if (IsWindowsScriptThere() == FALSE)
		file.WriteString(_T("\n .sct scripts disabled (Windows Script Host not found)\n"));

	file.Close();

	return TRUE;
}

/** @brief osvi.wProductType that works with MSVC6 headers */
static BYTE GetProductTypeFromOsvc(const OSVERSIONINFOEX & osvi)
{
	// wServicePackMinor (2 bytes)
	// wSuiteMask (2 bytes)
	// wProductType (1 byte)
	const BYTE * ptr = reinterpret_cast<const BYTE *>(&osvi.wServicePackMinor);
	return ptr[4];
}

/** @brief osvi.wSuiteMask that works with MSVC6 headers */
static WORD GetSuiteMaskFromOsvc(const OSVERSIONINFOEX & osvi)
{
	// wServicePackMinor (2 bytes)
	// wSuiteMask (2 bytes)
	const WORD * ptr = reinterpret_cast<const WORD *>(&osvi.wServicePackMinor);
	return ptr[1];
}

/** 
 * @brief Extract any helpful product details from version info
 */
static CString GetProductFromOsvi(const OSVERSIONINFOEX & osvi)
{
	CString sProduct;
	BYTE productType = GetProductTypeFromOsvc(osvi);
	WORD suiteMask = GetSuiteMaskFromOsvc(osvi);

	// Test for the workstation type.
	if ( productType == VER_NT_WORKSTATION )
	{
		if( osvi.dwMajorVersion == 4 )
			sProduct += _T( "Workstation 4.0 ");
		else if( suiteMask & VER_SUITE_PERSONAL )
			sProduct += _T( "Home Edition " );
		else
			sProduct += _T( "Professional " );
	}

	// Test for the server type.
	else if ( productType == VER_NT_SERVER )
	{
		if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if( suiteMask & VER_SUITE_DATACENTER )
				sProduct += _T( "Datacenter Edition " );
			else if( suiteMask & VER_SUITE_ENTERPRISE )
				sProduct += _T( "Enterprise Edition " );
			else if ( suiteMask == VER_SUITE_BLADE )
				sProduct += _T( "Web Edition " );
			else
				sProduct += _T( "Standard Edition " );
		}

		else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			if( suiteMask & VER_SUITE_DATACENTER )
				sProduct += _T( "Datacenter Server " );
			else if( suiteMask & VER_SUITE_ENTERPRISE )
				sProduct += _T( "Advanced Server " );
			else
				sProduct += _T( "Server " );
		}

		else  // Windows NT 4.0
		{
			if( suiteMask & VER_SUITE_ENTERPRISE )
				sProduct += _T("Server 4.0, Enterprise Edition " );
			else
				sProduct += _T( "Server 4.0 " );
		}
	}
	return sProduct;
}

#define REGBUFSIZE 1024
/** 
 * @brief Extract any helpful product details from registry (for WinNT)
 */
static CString GetNtProductFromRegistry(const OSVERSIONINFOEX & osvi)
{
	CString sProduct;

	HKEY hKey;
	TCHAR szProductType[REGBUFSIZE];
	DWORD dwBufLen=sizeof(szProductType)/sizeof(szProductType[0]);
	LONG lRet;

	lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
		0, KEY_QUERY_VALUE, &hKey );
	if( lRet != ERROR_SUCCESS )
		return _T("");

	lRet = RegQueryValueEx( hKey, _T("ProductType"), NULL, NULL,
		(LPBYTE) szProductType, &dwBufLen);
	RegCloseKey( hKey );

	if( (lRet != ERROR_SUCCESS) || (dwBufLen > REGBUFSIZE) )
		return _T("");

	if ( _tcsicmp( _T("WINNT"), szProductType) == 0 )
		sProduct = _T( "Workstation " );
	if ( _tcsicmp( _T("LANMANNT"), szProductType) == 0 )
		sProduct = _T( "Server " );
	if ( _tcsicmp( _T("SERVERNT"), szProductType) == 0 )
		sProduct = _T( "Advanced Server " );

	CString ver;
	ver.Format( _T("%d.%d "), osvi.dwMajorVersion, osvi.dwMinorVersion );
	sProduct += ver;
	return sProduct;
}

/** 
 * @brief Parse Windows version data to string
 */
CString CConfigLog::GetWindowsVer()
{
	OSVERSIONINFOEX osvi;
	CString sVersion;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !GetVersionEx ((OSVERSIONINFO *) &osvi) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
			return _T("");
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
		case VER_PLATFORM_WIN32_NT:

		// Test for the specific product family.
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			sVersion = _T("Microsoft Windows Server 2003 family, ");
		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			sVersion = _T("Microsoft Windows XP ");
		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			sVersion = _T("Microsoft Windows 2000 ");
		else if ( osvi.dwMajorVersion <= 4 )
			sVersion = _T("Microsoft Windows NT ");
		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
			sVersion = _T("Microsoft Windows Longhorn ");
		else
			sVersion.Format(_T("[? WindowsNT %d.%d] "), 
				osvi.dwMajorVersion, osvi.dwMinorVersion);

		if (osvi.dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX))
		{
			// Test for specific product on Windows NT 4.0 SP6 and later.
			CString sProduct = GetProductFromOsvi(osvi);
			sVersion += sProduct;
		}
		else
		{
			// Test for specific product on Windows NT 4.0 SP5 and earlier
			CString sProduct = GetNtProductFromRegistry(osvi);
			sVersion += sProduct;
		}

		// Display service pack (if any) and build number.
		if( osvi.dwMajorVersion == 4 &&
			_tcsicmp( osvi.szCSDVersion, _T("Service Pack 6") ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"),
				0, KEY_QUERY_VALUE, &hKey );
			CString ver;
			if( lRet == ERROR_SUCCESS )
				ver.Format(_T("Service Pack 6a (Build %d)"), osvi.dwBuildNumber & 0xFFFF );
			else // Windows NT 4.0 prior to SP6a
			{
				ver.Format(_T("%s (Build %d)"),
					osvi.szCSDVersion,
					osvi.dwBuildNumber & 0xFFFF);
			}

			sVersion += ver;
			RegCloseKey( hKey );
		}
		else // Windows NT 3.51 and earlier or Windows 2000 and later
		{
			CString ver;
			ver.Format( _T("%s (Build %d)"),
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
			sVersion += ver;
		}
		break;

	// Test for the Windows 95 product family.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			sVersion = _T("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				sVersion += _T("OSR2 " );
		}
		else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			sVersion = _T("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				sVersion += _T("SE " );
		}
		else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			sVersion = _T("Microsoft Windows Millennium Edition");
		}
		else
		{
			sVersion.Format(_T("[? Windows9x %d.%d] "), 
				osvi.dwMajorVersion, osvi.dwMinorVersion);
		}
		break;

	case VER_PLATFORM_WIN32s:
		sVersion = _T("Microsoft Win32s\n");
		break;

	default:
		sVersion.Format(_T(" [? Windows? %d.%d] "),
			osvi.dwMajorVersion, osvi.dwMinorVersion);
	}
	return sVersion;
}

/** 
 * @brief Determine some flags used when building
 */
CString CConfigLog::GetBuildFlags()
{
	CString flags;

#ifdef _DEBUG
	flags += " _DEBUG ";
#endif

#ifdef NDEBUG
	flags += " NDEBUG ";
#endif

#ifdef UNICODE
	flags += " UNICODE ";
#endif

#ifdef _UNICODE
	flags += " _UNICODE ";
#endif

	return flags;
}
