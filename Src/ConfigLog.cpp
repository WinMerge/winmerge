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
#include "version.h"
#include "dllver.h"
#include "DiffWrapper.h"
#include "ConfigLog.h"

/** 
 * @brief Return logfile name and path
 */
CString CConfigLog::GetFileName() const
{
	return m_sFileName;
}

/** 
 * @brief Write logfile
 */
BOOL CConfigLog::WriteLogFile()
{
	CStdioFile file;
	CVersionInfo version;
	CString tempPath;
	CString text;

	m_sFileName = _T("WinMerge.txt");
	CDiffWrapper::ReadDiffOptions(&diffOptions);

	// Get path to $temp/WinMerge.txt
	if (GetTempPath(MAX_PATH, tempPath.GetBuffer(MAX_PATH)))
	{
		tempPath.ReleaseBuffer();
		m_sFileName.Insert(-1, tempPath);
	}
	else
		return FALSE;

	if (!file.Open(m_sFileName, CFile::modeCreate | CFile::modeWrite))
		return FALSE;

	file.WriteString(_T("WinMerge configuration log\n"));
	file.WriteString(_T("--------------------------\n"));
	file.WriteString(_T("Saved to: "));
	file.WriteString(m_sFileName);
	file.WriteString(_T("\n* Please add this information (or attach this file)\n"));
	file.WriteString(_T("* when reporting bugs.\n"));

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

	file.WriteString(_T("\nWinMerge configuration:\n"));
	file.WriteString(_T(" Compare settings:\n"));
	file.WriteString(_T("  Ignore blank lines: "));
	if (diffOptions.bIgnoreBlankLines)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Ignore case: "));
	if (diffOptions.bIgnoreCase)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  EOL sensitive: "));
	if (diffOptions.bEolSensitive)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Whitespace compare: "));
	switch(diffOptions.nIgnoreWhitespace)
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

	file.WriteString(_T("\n Other settings:\n"));
	file.WriteString(_T("  Automatic rescan: "));
	if (miscSettings.bAutomaticRescan)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Simple EOL: "));
	if (miscSettings.bAllowMixedEol)
		file.WriteString(_T("No\n"));
	else
		file.WriteString(_T("Yes\n"));

	file.WriteString(_T("  Automatic scroll to 1st difference: "));
	if (miscSettings.bScrollToFirst)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Backup original file: "));
	if (miscSettings.bBackup)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("\n Show:\n"));
	file.WriteString(_T("  Identical files: "));
	if (viewSettings.bShowIdent)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Different files: "));
	if (viewSettings.bShowDiff)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Left Unique files: "));
	if (viewSettings.bShowUniqueLeft)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Right Unique files: "));
	if (viewSettings.bShowUniqueRight)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Binary files: "));
	if (viewSettings.bShowBinaries)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  Skipped files: "));
	if (viewSettings.bShowSkipped)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.WriteString(_T("  *.bak files: "));
	if (viewSettings.bHideBak)
		file.WriteString(_T("No\n"));
	else
		file.WriteString(_T("Yes\n"));

	file.WriteString(_T("\n View Whitespace: "));
	if (miscSettings.bViewWhitespace)
		file.WriteString(_T("Yes\n"));
	else
		file.WriteString(_T("No\n"));

	file.Close();

	return TRUE;
}

/** 
 * @brief Parse Windows version data to string
 */
CString CConfigLog::GetWindowsVer()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	CString sVersion;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
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
			sVersion = _T("Microsoft Windows Server&nbsp;2003 family, ");

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			sVersion = _T("Microsoft Windows XP ");

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			sVersion = _T("Microsoft Windows 2000 ");

		if ( osvi.dwMajorVersion <= 4 )
			sVersion = _T("Microsoft Windows NT ");

#if 0
		// Test for specific product on Windows NT 4.0 SP6 and later.
		if ( bOsVersionInfoEx )
		{
			// Test for the workstation type.
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				if( osvi.dwMajorVersion == 4 )
					sVersion += _T( "Workstation 4.0 ");
				else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					sVersion += _T( "Home Edition " );
				else
					sVersion += T( "Professional " );
			}

			// Test for the server type.
			else if ( osvi.wProductType == VER_NT_SERVER )
			{
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						sVersion += _T( "Datacenter Edition " );
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						sVersion += _T( "Enterprise Edition " );
					else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
						sVersion += _T( "Web Edition " );
					else
						sVersion += _T( "Standard Edition " );
				}

				else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				{
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						sVersion += _T( "Datacenter Server " );
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						sVersion += _T( "Advanced Server " );
					else
						sVersion += _T( "Server " );
				}

				else  // Windows NT 4.0
				{
					if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						sVersion += _T("Server 4.0, Enterprise Edition " );
					else
						sVersion += _T( "Server 4.0 " );
				}
			}
		}
		else  // Test for specific product on Windows NT 4.0 SP5 and earlier
		{
			HKEY hKey;
			TCHAR szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				_T"SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
				return FALSE;

			lRet = RegQueryValueEx( hKey, _T("ProductType"), NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
				return FALSE;

			RegCloseKey( hKey );

			if ( _tcsicmp( _T("WINNT"), szProductType) == 0 )
				sVersion += _T( "Workstation " );
			if ( _tcsicmp( _T("LANMANNT"), szProductType) == 0 )
				sVersion += _T( "Server " );
			if ( _tcsicmp( _T("SERVERNT"), szProductType) == 0 )
				sVersion += _T( "Advanced Server " );

			CString ver;
			ver.Format( _T("%d.%d "), osvi.dwMajorVersion, osvi.dwMinorVersion );
			sVersion += ver;
		}
#endif
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

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			sVersion = _T("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				sVersion += _T("SE " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			sVersion = _T("Microsoft Windows Millennium Edition");
		}
		break;

	/*
	case VER_PLATFORM_WIN32s:

		sVersion = _T("Microsoft Win32s\n");
		break;
	*/
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
