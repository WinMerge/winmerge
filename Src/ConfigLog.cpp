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
#include "7zcommon.h"


// Static function declarations
static bool LoadYesNoFromConfig(CfgSettings * cfgSettings, LPCTSTR name, BOOL * pbflag);



CConfigLog::CConfigLog()
: m_pCfgSettings(NULL)
{
}

CConfigLog::~CConfigLog()
{
	CloseFile();
}



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
	// do nothing if actually reading config file
	if (file.m_hFile == CFile::hFileNull) return;

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
 * @brief Return Windows font charset constant name from constant value, eg, FontCharsetName() => "Hebrew"
 */
static CString FontCharsetName(BYTE charset)
{
	switch(charset)
	{
	case ANSI_CHARSET: return _T("Ansi");
	case BALTIC_CHARSET: return _T("Baltic");
	case CHINESEBIG5_CHARSET: return _T("Chinese Big5");
	case DEFAULT_CHARSET: return _T("Default");
	case EASTEUROPE_CHARSET: return _T("East Europe");
	case GB2312_CHARSET: return _T("Chinese GB2312");
	case GREEK_CHARSET: return _T("Greek");
	case HANGUL_CHARSET: return _T("Korean Hangul");
	case MAC_CHARSET: return _T("Mac");
	case OEM_CHARSET: return _T("OEM");
	case RUSSIAN_CHARSET: return _T("Russian");
	case SHIFTJIS_CHARSET: return _T("Japanese Shift-JIS");
	case SYMBOL_CHARSET: return _T("Symbol");
	case TURKISH_CHARSET: return _T("Turkish");
	case VIETNAMESE_CHARSET: return _T("Vietnamese");
	case JOHAB_CHARSET: return _T("Korean Johab");
	case ARABIC_CHARSET: return _T("Arabic");
	case HEBREW_CHARSET: return _T("Hebrew");
	case THAI_CHARSET: return _T("Thai");
	default: return _T("Unknown");
	}
}

/**
 * @brief Write string item
 */
static void WriteItem(CStdioFile &file, int indent, LPCTSTR key, LPCTSTR value = 0)
{
	// do nothing if actually reading config file
	if (file.m_hFile == CFile::hFileNull) return;

	CString text;
	text.Format(value ? _T("%*.0s%s: %s\n") : _T("%*.0s%s:\n"), indent, key, key, value);
	file.WriteString(text);
}

/**
 * @brief Write int item
 */
static void WriteItem(CStdioFile &file, int indent, LPCTSTR key, long value)
{
	// do nothing if actually reading config file
	if (file.m_hFile == CFile::hFileNull) return;

	CString text;
	text.Format(_T("%*.0s%s: %ld\n"), indent, key, key, value);
	file.WriteString(text);
}

/**
 * @brief Write boolean item using keywords (Yes|No)
 */
void CConfigLog::WriteItemYesNo(int indent, LPCTSTR key, BOOL *pvalue)
{
	if (m_writing)
	{
		CString text;
		text.Format(_T("%*.0s%s: %s\n"), indent, key, key, *pvalue ? _T("Yes") : _T("No"));
		m_file.WriteString(text);
	}
	else
	{
		LoadYesNoFromConfig(m_pCfgSettings, key, pvalue);
	}
}

/**
 * @brief Same as WriteItemYesNo, except store Yes/No in reverse
 */
void CConfigLog::WriteItemYesNoInverted(int indent, LPCTSTR key, BOOL *pvalue)
{
	BOOL tempval = !(*pvalue);
	WriteItemYesNo(indent, key, &tempval);
	*pvalue = !(tempval);
}

/**
 * @brief Write out various possibly relevant windows locale information
 */
static void WriteLocaleSettings(CStdioFile & file, LCID locid, LPCTSTR title)
{
	// do nothing if actually reading config file
	if (file.m_hFile == CFile::hFileNull) return;

	WriteItem(file, 1, title);
	WriteItem(file, 2, _T("Def ANSI codepage"), GetLocaleString(locid, LOCALE_IDEFAULTANSICODEPAGE));
	WriteItem(file, 2, _T("Def OEM codepage"), GetLocaleString(locid, LOCALE_IDEFAULTCODEPAGE));
	WriteItem(file, 2, _T("Country"), GetLocaleString(locid, LOCALE_SENGCOUNTRY));
	WriteItem(file, 2, _T("Language"), GetLocaleString(locid, LOCALE_SENGLANGUAGE));
	WriteItem(file, 2, _T("Language code"), GetLocaleString(locid, LOCALE_ILANGUAGE));
	WriteItem(file, 2, _T("ISO Language code"), GetLocaleString(locid, LOCALE_SISO639LANGNAME));
}

/**
 * @brief Write version of a single executable file
 */
static void WriteVersionOf1(CStdioFile &file, int indent, LPTSTR path)
{
	// do nothing if actually reading config file
	if (file.m_hFile == CFile::hFileNull) return;

	LPTSTR name = PathFindFileName(path);
	CVersionInfo vi = path;
	CString text;
	text.Format
	(
		name == path
	?	_T("%*s%-20s %s=%u.%02u %s=%04u\n")
	:	_T("%*s%-20s %s=%u.%02u %s=%04u path=%s\n"),
		indent,
		// Tilde prefix for modules currently mapped into WinMerge
		GetModuleHandle(path) ? _T("~") : _T("")/*name*/,
		name,
		vi.m_dvi.cbSize > FIELD_OFFSET(DLLVERSIONINFO, dwMajorVersion)
	?	_T("dllversion")
	:	_T("version"),
		vi.m_dvi.dwMajorVersion,
		vi.m_dvi.dwMinorVersion,
		vi.m_dvi.cbSize > FIELD_OFFSET(DLLVERSIONINFO, dwBuildNumber)
	?	_T("dllbuild")
	:	_T("build"),
		vi.m_dvi.dwBuildNumber,
		path
	);
	file.WriteString(text);
}

/**
 * @brief Write version of a set of executable files
 */
static void WriteVersionOf(CStdioFile &file, int indent, LPTSTR path)
{
	LPTSTR name = PathFindFileName(path);
	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(path, &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			lstrcpy(name, ff.cFileName);
			WriteVersionOf1(file, indent, path);
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}
}

/**
 * @brief Write version of 7-Zip plugins and shell context menu handler
 */
static void WriteVersionOf7z(CStdioFile &file, LPTSTR path)
{
	lstrcat(path, _T("\\7-zip*.dll"));
	LPTSTR pattern = PathFindFileName(path);
	WriteVersionOf(file, 2, path);
	WriteItem(file, 2, _T("Codecs"));
	lstrcpy(pattern, _T("codecs\\*.dll"));
	WriteVersionOf(file, 3, path);
	WriteItem(file, 2, _T("Formats"));
	lstrcpy(pattern, _T("formats\\*.dll"));
	WriteVersionOf(file, 3, path);
}

/**
 * @brief Write archive support stuff
 */
static void WriteArchiveSupport(CStdioFile &file)
{
	DWORD registered = VersionOf7z(FALSE);
	DWORD standalone = VersionOf7z(TRUE);
	TCHAR path[MAX_PATH];
	DWORD type = 0;
	DWORD size = sizeof path;

	WriteItem(file, 0, _T("Archive support"));
	WriteItem(file, 1, _T("Enable"),
		AfxGetApp()->GetProfileInt(_T("Merge7z"), _T("Enable"), 0));

	wsprintf(path, _T("%u.%02u"), UINT HIWORD(registered), UINT LOWORD(registered));
	WriteItem(file, 1, _T("7-Zip software installed on your computer"), path);
	static const TCHAR szSubKey[] = _T("Software\\7-Zip");
	static const TCHAR szValue[] = _T("Path");
	SHGetValue(HKEY_LOCAL_MACHINE, szSubKey, szValue, &type, path, &size);
	WriteVersionOf7z(file, path);

	wsprintf(path, _T("%u.%02u"), UINT HIWORD(standalone), UINT LOWORD(standalone));
	WriteItem(file, 1, _T("7-Zip components for standalone operation"), path);
	GetModuleFileName(0, path, countof(path));
	LPTSTR pattern = PathFindFileName(path);
	PathRemoveFileSpec(path);
	WriteVersionOf7z(file, path);

	WriteItem(file, 1, _T("Merge7z plugins on path"));
	lstrcpy(pattern, _T("Merge7z*.dll"));
	WriteVersionOf(file, 2, path);
	// now see what's on the path:
	if (DWORD cchPath = GetEnvironmentVariable(_T("path"), 0, 0))
	{
		static const TCHAR cSep[] = _T(";");
		LPTSTR pchPath = new TCHAR[cchPath];
		GetEnvironmentVariable(_T("PATH"), pchPath, cchPath);
		LPTSTR pchItem = pchPath;
		while (int cchItem = StrCSpn(pchItem += StrSpn(pchItem, cSep), cSep))
		{
			if (cchItem < MAX_PATH)
			{
				CopyMemory(path, pchItem, cchItem*sizeof*pchItem);
				path[cchItem] = 0;
				PathAppend(path, _T("Merge7z*.dll"));
				WriteVersionOf(file, 2, path);
			}
			pchItem += cchItem;
		}
		delete[] pchPath;
	}
}

struct NameMap { int ival; LPCTSTR sval; };
/**
 * @brief Write boolean item using keywords (Yes|No)
 */
void CConfigLog::
WriteItemWhitespace(int indent, LPCTSTR key, int *pvalue)
{
	static NameMap namemap[] = {
		{ WHITESPACE_COMPARE_ALL, _T("Compare all") }
		, { WHITESPACE_IGNORE_CHANGE, _T("Ignore change") }
		, { WHITESPACE_IGNORE_ALL, _T("Ignore all") }
	};

	if (m_writing)
	{
		CString text = _T("Unknown");
		for (int i=0; i<sizeof(namemap)/sizeof(namemap[0]); ++i)
		{
			if (*pvalue == namemap[i].ival)
				text = namemap[i].sval;
		}
		WriteItem(m_file, indent, key, text);
	}
	else
	{
		*pvalue = namemap[0].ival;
		CString svalue = GetValueFromConfig(key);
		for (int i=0; i<sizeof(namemap)/sizeof(namemap[0]); ++i)
		{
			if (svalue == namemap[i].sval)
				*pvalue = namemap[i].ival;
		}
	}
}


/** 
 * @brief Write logfile
 */
BOOL CConfigLog::DoFile(bool writing, CString &sError)
{
	CFileException e;
	CVersionInfo version;
	CString text;

	m_writing = writing;

	if (writing)
	{
		m_sFileName = _T("WinMerge.txt");

		// Get path to $temp/WinMerge.txt
		m_sFileName.Insert(0, paths_GetTempPath());

		if (!m_file.Open(m_sFileName, CFile::modeCreate | CFile::modeWrite))
		{
			TCHAR szError[1024];
			e.GetErrorMessage(szError, 1024);
			sError = szError;
			return FALSE;
		}
	}

// Begin log
	FileWriteString(_T("WinMerge configuration log\n"));
	FileWriteString(_T("--------------------------\n"));
	FileWriteString(_T("Saved to: "));
	FileWriteString(m_sFileName);
	FileWriteString(_T("\n* Please add this information (or attach this file)\n"));
	FileWriteString(_T("* when reporting bugs.\n"));
	FileWriteString(_T("Module names prefixed with tilda (~) are currently loaded in WinMerge process.\n"));

// Platform stuff
	FileWriteString(_T("\n\nVersion information:\n"));
	FileWriteString(_T(" WinMerge.exe: "));
	FileWriteString(version.GetFixedProductVersion());

	CString privBuild = version.GetPrivateBuild();
	if (!privBuild.IsEmpty())
	{
		FileWriteString(_T(" - Private build: "));
		FileWriteString(privBuild);
	}

	text = GetBuildFlags();
	FileWriteString(_T("\n Build config: "));
	FileWriteString(text);

	FileWriteString(_T("\n Windows: "));
	text = GetWindowsVer();
	FileWriteString(text);

	FileWriteString(_T("\n"));
	WriteVersionOf1(m_file, 1, _T("COMCTL32.dll"));
	WriteVersionOf1(m_file, 1, _T("ShellExtension.dll"));
	WriteVersionOf1(m_file, 1, _T("ShellExtensionU.dll"));

	FileWriteString(_T("\n"));
	WriteArchiveSupport(m_file);
// WinMerge settings
	FileWriteString(_T("\nWinMerge configuration:\n"));
	FileWriteString(_T(" Compare settings:\n"));

	WriteItemYesNo(2, _T("Ignore blank lines"), &m_diffOptions.bIgnoreBlankLines);
	WriteItemYesNo(2, _T("Ignore case"), &m_diffOptions.bIgnoreCase);
	WriteItemYesNoInverted(2, _T("Ignore carriage return differences"), &m_diffOptions.bEolSensitive);

	WriteItemWhitespace(2, _T("Whitespace compare"), &m_diffOptions.nIgnoreWhitespace);

	WriteItemYesNo(2, _T("Detect moved blocks"), &m_miscSettings.bMovedBlocks);

	FileWriteString(_T("\n Other settings:\n"));
	WriteItemYesNo(2, _T("Automatic rescan"), &m_miscSettings.bAutomaticRescan);
	WriteItemYesNoInverted(2, _T("Simple EOL"), &m_miscSettings.bAllowMixedEol);
	WriteItemYesNo(2, _T("Automatic scroll to 1st difference"), &m_miscSettings.bScrollToFirst);
	WriteItemYesNo(2, _T("Backup original file"), &m_miscSettings.bBackup);

	FileWriteString(_T("\n Show:\n"));
	WriteItemYesNo(2, _T("Identical files"), &m_viewSettings.bShowIdent);
	WriteItemYesNo(2, _T("Different files"), &m_viewSettings.bShowDiff);
	WriteItemYesNo(2, _T("Left Unique files"), &m_viewSettings.bShowUniqueLeft);
	WriteItemYesNo(2, _T("Right Unique files"), &m_viewSettings.bShowUniqueRight);
	WriteItemYesNo(2, _T("Binary files"), &m_viewSettings.bShowBinaries);
	WriteItemYesNo(2, _T("Skipped files"), &m_viewSettings.bShowSkipped);

	FileWriteString(_T("\n"));
	WriteItemYesNo(1, _T("View Whitespace"), &m_miscSettings.bViewWhitespace);
	
// Font settings
	FileWriteString(_T("\n Font:\n"));
	FileWriteString(Fmt(_T("  Font facename: %s\n"), m_fontSettings.sFacename));
	FileWriteString(Fmt(_T("  Font charset: %d (%s)\n"), m_fontSettings.nCharset, 
		FontCharsetName(m_fontSettings.nCharset)));

// System settings
	FileWriteString(_T("\nSystem settings:\n"));
	FileWriteString(_T(" codepage settings:\n"));
	WriteItem(m_file, 2, _T("ANSI codepage"), GetACP());
	WriteItem(m_file, 2, _T("OEM codepage"), GetOEMCP());
#ifndef UNICODE
	WriteItem(m_file, 2, _T("multibyte codepage"), _getmbcp());
#endif
	WriteLocaleSettings(m_file, GetThreadLocale(), _T("Locale (Thread)"));
	WriteLocaleSettings(m_file, LOCALE_USER_DEFAULT, _T("Locale (User)"));
	WriteLocaleSettings(m_file, LOCALE_SYSTEM_DEFAULT, _T("Locale (System)"));

// Codepage settings
	WriteItemYesNo(0, _T("Detect codepage automatically for RC and HTML files"), &m_cpSettings.bDetectCodepage);
	WriteItem(m_file, 1, _T("unicoder codepage"), getDefaultCodepage());

// Plugins
	FileWriteString(_T("\nPlugins: "));
	FileWriteString(_T("\n Unpackers: "));
	WritePluginsInLogFile(L"FILE_PACK_UNPACK", m_file);
	WritePluginsInLogFile(L"BUFFER_PACK_UNPACK", m_file);
	FileWriteString(_T("\n Prediffers: "));
	WritePluginsInLogFile(L"FILE_PREDIFF", m_file);
	WritePluginsInLogFile(L"BUFFER_PREDIFF", m_file);
	FileWriteString(_T("\n Editor scripts: "));
	WritePluginsInLogFile(L"EDITOR_SCRIPT", m_file);
	if (IsWindowsScriptThere() == FALSE)
		FileWriteString(_T("\n .sct scripts disabled (Windows Script Host not found)\n"));

	CloseFile();

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
 * @brief Return string representation of build flags (for reporting in config log)
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

/** 
 * @brief  Collection of configuration settings found in config log (name/value map)
 */
class CfgSettings
{
public:
	CfgSettings() { m_settings.InitHashTable(411); }
	void Add(LPCTSTR name, LPCTSTR value) { m_settings.SetAt(name, value); }
	bool Lookup(LPCTSTR name, CString & value) { return !!m_settings.Lookup(name, value); }
private:
	CMapStringToString m_settings;
};

/**
 * @brief  Lookup named setting in cfgSettings, and if found, set pbflag accordingly
 */
static bool
LoadYesNoFromConfig(CfgSettings * cfgSettings, LPCTSTR name, BOOL * pbflag)
{
	CString value;
	if (cfgSettings->Lookup(name, value))
	{
		if (value == _T("Yes"))
		{
			*pbflag = TRUE;
			return true;
		}
		else if (value == _T("No"))
		{
			*pbflag = FALSE;
			return true;
		}
	}
	return false;
}



BOOL
CConfigLog::WriteLogFile(CString &sError)
{
	CloseFile();

	bool writing = true;
	return DoFile(writing, sError);
}

void
CConfigLog::ReadLogFile(const CString & Filepath)
{
	CloseFile();

	bool writing = false;
	CString sError;
	m_pCfgSettings = new CfgSettings;
	if (!ParseSettings(Filepath)) return;
	DoFile(writing, sError);
}

/// Write line to file (if writing configuration log)
void
CConfigLog::FileWriteString(LPCTSTR lpsz)
{
	if (m_writing)
		m_file.WriteString(lpsz);
}

/**
 * @brief Close any open file
 */
void
CConfigLog::CloseFile()
{
	if (m_file.m_hFile != CFile::hFileNull)
		m_file.Close();
	delete m_pCfgSettings;
	m_pCfgSettings = 0;
}

/**
 * @brief  Store all name:value strings from file into m_pCfgSettings
 */
bool
CConfigLog::ParseSettings(const CString & Filepath)
{
	CStdioFile file;
	if (!file.Open(Filepath, CFile::modeRead))
		return false;

	CString strline;
	while (file.ReadString(strline))
	{
		int colon = strline.Find(':');
		if (colon > 0)
		{
			CString name = strline.Left(colon);
			CString value = strline.Mid(colon+1);
			name.TrimLeft();
			name.TrimRight();
			value.TrimLeft();
			value.TrimRight();
			m_pCfgSettings->Add(name, value);
		}
	}
	file.Close();
	return true;


}

CString
CConfigLog::GetValueFromConfig(const CString & key)
{
	CString value;
	if (m_pCfgSettings->Lookup(key, value))
	{
		return value;
	}
	return _T("");
}
