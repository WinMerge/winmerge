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
// ID line follows -- this is updated by SVN
// $Id: ConfigLog.cpp 7082 2010-01-03 22:15:50Z sdottaka $

#include "ConfigLog.h"
#include <cassert>
#include <boost/scoped_array.hpp>
#include <windows.h>
#include <mbctype.h>
#include "Constants.h"
#include "version.h"
#include "UniFile.h"
#include "Plugins.h"
#include "paths.h"
#include "unicoder.h"
#include "codepage.h"
#include "Environment.h"
#include "MergeApp.h"
#include "OptionsMgr.h"
#include "TempFile.h"
#include "UniFile.h"

BOOL NTAPI IsMerge7zEnabled();
DWORD NTAPI VersionOf7z(BOOL bLocal = FALSE);

CConfigLog::CConfigLog()
: m_pfile(new UniStdioFile())
{
}

CConfigLog::~CConfigLog()
{
	CloseFile();
}



/** 
 * @brief Return logfile name and path
 */
String CConfigLog::GetFileName() const
{
	return m_sFileName;
}

/** 
 * @brief Write plugin names
 */
void CConfigLog::WritePluginsInLogFile(LPCWSTR transformationEvent)
{
	// get an array with the available scripts
	PluginArray * piPluginArray; 

	piPluginArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(transformationEvent);

	int iPlugin;
	for (iPlugin = 0 ; iPlugin < piPluginArray->size() ; iPlugin++)
	{
		const PluginInfoPtr& plugin = piPluginArray->at(iPlugin);
		m_pfile->WriteString(_T("\r\n  "));
		m_pfile->WriteString(plugin->m_name);
		m_pfile->WriteString(_T(" ["));
		m_pfile->WriteString(plugin->m_filepath);
		m_pfile->WriteString(_T("]"));
	}
}

/**
 * @brief String wrapper around API call GetLocaleInfo
 */
static String GetLocaleString(LCID locid, LCTYPE lctype)
{
	TCHAR buffer[512];
	if (!GetLocaleInfo(locid, lctype, buffer, sizeof(buffer)/sizeof(buffer[0])))
		buffer[0] = 0;
	return buffer;
}

/**
 * @brief Write string item
 */
void CConfigLog::WriteItem(int indent, const String& key, LPCTSTR value)
{
	String text = string_format(value ? _T("%*.0s%s: %s\r\n") : _T("%*.0s%s:\r\n"), indent, key.c_str(), key.c_str(), value);
	m_pfile->WriteString(text);
}

/**
 * @brief Write string item
 */
void CConfigLog::WriteItem(int indent, const String& key, const String &str)
{
	WriteItem(indent, key, str.c_str());
}

/**
 * @brief Write int item
 */
void CConfigLog::WriteItem(int indent, const String& key, long value)
{
	String text = string_format(_T("%*.0s%s: %ld\r\n"), indent, key.c_str(), key.c_str(), value);
	m_pfile->WriteString(text);
}

/**
 * @brief Write boolean item using keywords (Yes|No)
 */
void CConfigLog::WriteItemYesNo(int indent, const String& key, bool *pvalue)
{
	String text = string_format(_T("%*.0s%s: %s\r\n"), indent, key.c_str(), key.c_str(), *pvalue ? _T("Yes") : _T("No"));
	m_pfile->WriteString(text);
}

/**
 * @brief Write out various possibly relevant windows locale information
 */
void CConfigLog::WriteLocaleSettings(unsigned locid, const String& title)
{
	WriteItem(1, title);
	WriteItem(2, _T("Def ANSI codepage"), GetLocaleString(locid, LOCALE_IDEFAULTANSICODEPAGE));
	WriteItem(2, _T("Def OEM codepage"), GetLocaleString(locid, LOCALE_IDEFAULTCODEPAGE));
	WriteItem(2, _T("Country"), GetLocaleString(locid, LOCALE_SENGCOUNTRY));
	WriteItem(2, _T("Language"), GetLocaleString(locid, LOCALE_SENGLANGUAGE));
	WriteItem(2, _T("Language code"), GetLocaleString(locid, LOCALE_ILANGUAGE));
	WriteItem(2, _T("ISO Language code"), GetLocaleString(locid, LOCALE_SISO639LANGNAME));
}

/**
 * @brief Write version of a single executable file
 */
void CConfigLog::WriteVersionOf1(int indent, const String& path)
{
	String name = paths_FindFileName(path);
	CVersionInfo vi(path.c_str(), TRUE);
	String text = string_format
	(
		name == path
	?	_T("%*s%-20s %s=%u.%02u %s=%04u\r\n")
	:	_T("%*s%-20s %s=%u.%02u %s=%04u path=%s\r\n"),
		indent,
		// Tilde prefix for modules currently mapped into WinMerge
		GetModuleHandle(path.c_str()) ? _T("~") : _T("")/*name*/,
		name.c_str(),
		vi.m_dvi.cbSize > FIELD_OFFSET(DLLVERSIONINFO, dwMajorVersion)
	?	_T("dllversion")
	:	_T("version"),
		vi.m_dvi.dwMajorVersion,
		vi.m_dvi.dwMinorVersion,
		vi.m_dvi.cbSize > FIELD_OFFSET(DLLVERSIONINFO, dwBuildNumber)
	?	_T("dllbuild")
	:	_T("build"),
		vi.m_dvi.dwBuildNumber,
		path.c_str()
	);
	m_pfile->WriteString(text);
}

/**
 * @brief Write version of a set of executable files
 */
void CConfigLog::WriteVersionOf(int indent, const String& path)
{
	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(path.c_str(), &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		String dir = paths_GetPathOnly(path);
		do
		{
			WriteVersionOf1(indent, paths_ConcatPath(dir, ff.cFileName));
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}
}

/**
 * @brief Write version of 7-Zip plugins and shell context menu handler
 */
void CConfigLog::WriteVersionOf7z(const String& dir)
{
	WriteVersionOf(2, paths_ConcatPath(dir, _T("7-zip*.dll")));
	WriteItem(2, _T("Codecs"));
	WriteVersionOf(3, paths_ConcatPath(dir, _T("codecs\\*.dll")));
	WriteItem(2, _T("Formats"));
	WriteVersionOf(3, paths_ConcatPath(dir, _T("formats\\*.dll")));
}

/**
 * @brief Write archive support stuff
 */
void CConfigLog::WriteArchiveSupport()
{
	DWORD registered = VersionOf7z(FALSE);
	DWORD standalone = VersionOf7z(TRUE);
	TCHAR path[MAX_PATH];
	DWORD type = 0;
	DWORD size = sizeof path;

	WriteItem(0, _T("Archive support"));
	WriteItem(1, _T("Enable"), IsMerge7zEnabled());

	wsprintf(path, _T("%u.%02u"), UINT HIWORD(registered), UINT LOWORD(registered));
	WriteItem(1, _T("7-Zip software installed on your computer"), path);
	static const TCHAR szSubKey[] = _T("Software\\7-Zip");
	static const TCHAR szValue[] = _T("Path");
	SHGetValue(HKEY_LOCAL_MACHINE, szSubKey, szValue, &type, path, &size);
	WriteVersionOf7z(path);

	wsprintf(path, _T("%u.%02u"), UINT HIWORD(standalone), UINT LOWORD(standalone));
	WriteItem(1, _T("7-Zip components for standalone operation"), path);
	GetModuleFileName(0, path, sizeof(path)/sizeof(path[0]));
	WriteVersionOf7z(paths_GetPathOnly(path));

	WriteItem(1, _T("Merge7z plugins on path"));

	WriteVersionOf(2, paths_ConcatPath(paths_GetPathOnly(path), _T("Merge7z*.dll")));
	// now see what's on the path:
	if (DWORD cchPath = GetEnvironmentVariable(_T("path"), 0, 0))
	{
		static const TCHAR cSep[] = _T(";");
		boost::scoped_array<TCHAR> pchPath(new TCHAR[cchPath]);
		GetEnvironmentVariable(_T("PATH"), pchPath.get(), cchPath);
		LPTSTR pchItem = &pchPath[0];
		while (int cchItem = StrCSpn(pchItem += StrSpn(pchItem, cSep), cSep))
		{
			if (cchItem < MAX_PATH)
			{
				CopyMemory(path, pchItem, cchItem*sizeof*pchItem);
				path[cchItem] = 0;
				PathAppend(path, _T("Merge7z*.dll"));
				WriteVersionOf(2, path);
			}
			pchItem += cchItem;
		}
	}
}

/**
 * @brief Write winmerge configuration
 */
void CConfigLog::WriteWinMergeConfig(void)
{
	TempFile tmpfile;
	tmpfile.Create();
	GetOptionsMgr()->ExportOptions(tmpfile.GetPath());
	UniMemFile ufile;
	if (!ufile.OpenReadOnly(tmpfile.GetPath()))
		return;
	String line;
	bool lossy;
	while (ufile.ReadString(line, &lossy)) 
	{
		FileWriteString(line + _T("\r\n"));
	}
}

/** 
 * @brief Write logfile
 */
bool CConfigLog::DoFile(String &sError)
{
	CVersionInfo version;
	String text;

	String sFileName = paths_ConcatPath(env_GetMyDocuments(), WinMergeDocumentsFolder);
	paths_CreateIfNeeded(sFileName);
	m_sFileName = paths_ConcatPath(sFileName, _T("WinMerge.txt"));

#ifdef _UNICODE
	if (!m_pfile->OpenCreateUtf8(m_sFileName))
#else
	if (!m_pfile->OpenCreate(m_sFileName))
#endif
	{
		const UniFile::UniError &err = m_pfile->GetLastUniError();
		sError = err.GetError();
		return false;
	}
#ifdef _UNICODE
	m_pfile->SetBom(true);
	m_pfile->WriteBom();
#endif

// Begin log
	FileWriteString(_T("WinMerge configuration log\r\n"));
	FileWriteString(_T("--------------------------\r\n"));
	FileWriteString(_T("Saved to: "));
	FileWriteString(m_sFileName);
	FileWriteString(_T("\r\n* Please add this information (or attach this file)\r\n"));
	FileWriteString(_T("* when reporting bugs.\r\n"));
	FileWriteString(_T("Module names prefixed with tilda (~) are currently loaded in WinMerge process.\r\n"));

// Platform stuff
	FileWriteString(_T("\r\n\r\nVersion information:\r\n"));
	FileWriteString(_T(" WinMerge.exe: "));
	FileWriteString(version.GetFixedProductVersion());

	String privBuild = version.GetPrivateBuild();
	if (!privBuild.empty())
	{
		FileWriteString(_T(" - Private build: "));
		FileWriteString(privBuild);
	}

	text = GetBuildFlags();
	FileWriteString(_T("\r\n Build config: "));
	FileWriteString(text);

	LPCTSTR szCmdLine = ::GetCommandLine();
	assert(szCmdLine != NULL);

	// Skip the quoted executable file name.
	if (szCmdLine != NULL)
	{
		szCmdLine = _tcschr(szCmdLine, '"');
		if (szCmdLine != NULL)
		{
			szCmdLine += 1; // skip the opening quote.
			szCmdLine = _tcschr(szCmdLine, '"');
			if (szCmdLine != NULL)
			{
				szCmdLine += 1; // skip the closing quote.
			}
		}
	}

	// The command line include a space after the executable file name,
	// which mean that empty command line will have length of one.
	if (lstrlen(szCmdLine) < 2)
	{
		szCmdLine = _T(" none");
	}

	FileWriteString(_T("\r\n Command Line: "));
	FileWriteString(szCmdLine);

	FileWriteString(_T("\r\n Windows: "));
	text = GetWindowsVer();
	FileWriteString(text);

	FileWriteString(_T("\r\n"));
	WriteVersionOf1(1, _T("COMCTL32.dll"));
	WriteVersionOf1(1, _T("shlwapi.dll"));
	WriteVersionOf1(1, _T("MergeLang.dll"));
	WriteVersionOf1(1, _T("ShellExtension.dll"));
	WriteVersionOf1(1, _T("ShellExtensionU.dll"));
	WriteVersionOf1(1, _T("ShellExtensionX64.dll"));

// System settings
	FileWriteString(_T("\r\nSystem settings:\r\n"));
	FileWriteString(_T(" codepage settings:\r\n"));
	WriteItem(2, _T("ANSI codepage"), GetACP());
	WriteItem(2, _T("OEM codepage"), GetOEMCP());
#ifndef UNICODE
	WriteItem(2, _T("multibyte codepage"), _getmbcp());
#endif
	WriteLocaleSettings(GetThreadLocale(), _T("Locale (Thread)"));
	WriteLocaleSettings(LOCALE_USER_DEFAULT, _T("Locale (User)"));
	WriteLocaleSettings(LOCALE_SYSTEM_DEFAULT, _T("Locale (System)"));

// Plugins
	FileWriteString(_T("\r\nPlugins:\r\n"));
	FileWriteString(_T(" Unpackers: "));
	WritePluginsInLogFile(L"FILE_PACK_UNPACK");
	WritePluginsInLogFile(L"BUFFER_PACK_UNPACK");
	FileWriteString(_T("\r\n Prediffers: "));
	WritePluginsInLogFile(L"FILE_PREDIFF");
	WritePluginsInLogFile(L"BUFFER_PREDIFF");
	FileWriteString(_T("\r\n Editor scripts: "));
	WritePluginsInLogFile(L"EDITOR_SCRIPT");
	if (IsWindowsScriptThere() == FALSE)
		FileWriteString(_T("\r\n .sct scripts disabled (Windows Script Host not found)\r\n"));

	FileWriteString(_T("\r\n\r\n"));
	WriteArchiveSupport();

// WinMerge settings
	FileWriteString(_T("\r\nWinMerge configuration:\r\n"));
	WriteWinMergeConfig();

	CloseFile();

	return true;
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
static String GetProductFromOsvi(const OSVERSIONINFOEX & osvi)
{
	String sProduct;
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
static String GetNtProductFromRegistry(const OSVERSIONINFOEX & osvi)
{
	String sProduct;

	HKEY hKey;
	TCHAR szProductType[REGBUFSIZE];
	DWORD dwBufLen = sizeof(szProductType)/sizeof(szProductType[0]);
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

	String ver = string_format( _T("%d.%d "), osvi.dwMajorVersion, osvi.dwMinorVersion );
	sProduct += ver;
	return sProduct;
}

/** 
 * @brief Parse Windows version data to string.
 * See info about how to determine Windows versions from URL:
 * http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
 * @return String describing Windows version.
 */
String CConfigLog::GetWindowsVer()
{
	OSVERSIONINFOEX osvi;
	String sVersion;

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
		{
			if (osvi.wProductType == VER_NT_WORKSTATION)
				sVersion = _T("Microsoft Windows Vista ");
			else
				sVersion = _T("Microsoft Windows Server 2008 ");
		}
		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
		{
			if (osvi.wProductType == VER_NT_WORKSTATION)
				sVersion = _T("Microsoft Windows 7 ");
			else
				sVersion = _T("Microsoft Windows Server 2008 R2 ");
		}
		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 )
		{
			if (osvi.wProductType == VER_NT_WORKSTATION)
				sVersion = _T("Microsoft Windows 8 ");
			else
				sVersion = _T("Microsoft Windows Server 2012 ");
		}
		else
			sVersion = string_format(_T("[? WindowsNT %d.%d] "), 
				osvi.dwMajorVersion, osvi.dwMinorVersion);

		if (osvi.dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX))
		{
			// Test for specific product on Windows NT 4.0 SP6 and later.
			String sProduct = GetProductFromOsvi(osvi);
			sVersion += sProduct;
		}
		else
		{
			// Test for specific product on Windows NT 4.0 SP5 and earlier
			String sProduct = GetNtProductFromRegistry(osvi);
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
			String ver;
			if( lRet == ERROR_SUCCESS )
				ver = string_format(_T("Service Pack 6a (Build %d)"), osvi.dwBuildNumber & 0xFFFF );
			else // Windows NT 4.0 prior to SP6a
			{
				ver = string_format(_T("%s (Build %d)"),
					osvi.szCSDVersion,
					osvi.dwBuildNumber & 0xFFFF);
			}

			sVersion += ver;
			RegCloseKey( hKey );
		}
		else // Windows NT 3.51 and earlier or Windows 2000 and later
		{
			String ver = string_format( _T("%s (Build %d)"),
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
			sVersion = string_format(_T("[? Windows9x %d.%d] "), 
				osvi.dwMajorVersion, osvi.dwMinorVersion);
		}
		break;

	case VER_PLATFORM_WIN32s:
		sVersion = _T("Microsoft Win32s\r\n");
		break;

	default:
		sVersion = string_format(_T(" [? Windows? %d.%d] "),
			osvi.dwMajorVersion, osvi.dwMinorVersion);
	}
	return sVersion;
}

/** 
 * @brief Return string representation of build flags (for reporting in config log)
 */
String CConfigLog::GetBuildFlags() const
{
	String flags;

#ifdef _DEBUG
	flags += _T(" _DEBUG ");
#endif

#ifdef NDEBUG
	flags += _T(" NDEBUG ");
#endif

#ifdef UNICODE
	flags += _T(" UNICODE ");
#endif

#ifdef _UNICODE
	flags += _T(" _UNICODE ");
#endif

#ifdef _MBCS
	flags += _T(" _MBCS ");
#endif

#ifdef WIN64
	flags += _T(" WIN64 ");
#endif

	return flags;
}

bool CConfigLog::WriteLogFile(String &sError)
{
	CloseFile();

	return DoFile(sError);
}

/// Write line to file (if writing configuration log)
void
CConfigLog::FileWriteString(const String& lpsz)
{
	m_pfile->WriteString(lpsz);
}

/**
 * @brief Close any open file
 */
void
CConfigLog::CloseFile()
{
	if (m_pfile->IsOpen())
		m_pfile->Close();
}

