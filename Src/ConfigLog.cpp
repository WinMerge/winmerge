// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  ConfigLog.cpp
 *
 * @brief CConfigLog implementation
 */

#include "pch.h"
#include "ConfigLog.h"
#include <cassert>
#include <windows.h>
#include <memory>
#include "Constants.h"
#include "VersionInfo.h"
#include "UniFile.h"
#include "Plugins.h"
#include "TFile.h"
#include "paths.h"
#include "locality.h"
#include "unicoder.h"
#include "Environment.h"
#include "MergeApp.h"
#include "OptionsMgr.h"
#include "TempFile.h"
#include "UniFile.h"
#include "RegKey.h"

CConfigLog::CConfigLog()
: m_pfile(new UniStdioFile())
{
}

CConfigLog::~CConfigLog()
{
	CloseFile();
}


/** 
 * @brief Get details of the Compiler Version from _MSC_VER, etc.
 *		Infer the Visual Studio version
 */
static String GetCompilerVersion()
{
	String sVisualStudio = _T(""); 
#ifdef _MSC_VER
#if     _MSC_VER <  1800
	# error "** Unknown OLD Version of Visual Studio **"
#elif	_MSC_VER == 1800
	sVisualStudio = _T("VS.2013 (12.0) - "); 
#elif	_MSC_VER == 1900
	sVisualStudio = _T("VS.2015 (14.0) - "); 
#elif	_MSC_VER == 1910
	sVisualStudio = _T("VS.2017 (15.0) - "); 
#elif	_MSC_VER == 1911
	sVisualStudio = _T("VS.2017 (15.3) - "); 
#elif	_MSC_VER >= 1912 && _MSC_VER <  1920
	sVisualStudio = strutils::format(_T("VS.2017 (15.%d) - "), 5 + (_MSC_VER - 1912));
#elif   _MSC_VER >= 1920 && _MSC_VER <  1930
	sVisualStudio = strutils::format(_T("VS.2019 (16.%d) - "), (_MSC_VER - 1920));
#elif   _MSC_VER >= 1930 && _MSC_VER <  2000
	sVisualStudio = strutils::format(_T("VS.2022 (17.%d) - "), (_MSC_VER - 1930));
#elif	_MSC_VER >= 2000
	# error "** Unknown NEW Version of Visual Studio **"
#endif
#endif

	return strutils::format(_T("%sC/C++ Compiler %02i.%02i.%05i.%i"),
		sVisualStudio,
		(int)(_MSC_VER / 100), (int)(_MSC_VER % 100), (int)(_MSC_FULL_VER % 100000), _MSC_BUILD
	);
}

/** 
 * @brief Get the Modified time of fully qualified file path and name
 */
static String GetLastModified(const String &path) 
{
	String sPath2 = path;
	if (sPath2[0] == '.')
	{
		CVersionInfo EXEversion;
		String sEXEPath = paths::GetPathOnly(paths::GetLongPath(EXEversion.GetFullFileName(), false));
		sPath2 = sEXEPath + _T("\\") + sPath2;
	}
	TFile file(sPath2);

	String sModifiedTime = _T("");
	if (file.exists())
	{
		Poco::Timestamp mtime(file.getLastModified());

		const int64_t r = (mtime.epochTime());
		sModifiedTime = locality::TimeString(&r);
	}
	return sModifiedTime;
}

/** 
 * @brief Write plugin names
 */
void CConfigLog::WritePluginsInLogFile(const wchar_t *transformationEvent)
{
	CVersionInfo EXEversion;
	String sEXEPath = paths::GetPathOnly(paths::GetLongPath(EXEversion.GetFullFileName(), false));

	// get an array with the available scripts
	PluginArray * piPluginArray; 

	piPluginArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(transformationEvent);

	for (size_t iPlugin = 0 ; iPlugin < piPluginArray->size() ; iPlugin++)
	{
		const PluginInfoPtr& plugin = piPluginArray->at(iPlugin);
		String sPluginText;
		if (plugin->m_filepath.find(':') != String::npos)
		{
			String sFileName = paths::GetLongPath(plugin->m_filepath);
			if (sFileName.length() > sEXEPath.length())
				if (sFileName.substr(0, sEXEPath.length()) == sEXEPath)
					sFileName = _T(".") + sFileName.erase(0, sEXEPath.length());

			String sModifiedTime = _T("");
			sModifiedTime = GetLastModified(plugin->m_filepath);
			if (!sModifiedTime.empty())
				sModifiedTime = _T("[") + sModifiedTime + _T("]");

			sPluginText = strutils::format
			(_T("\r\n  %s%-36s path=%s  %s"),
				plugin->m_disabled ? _T("!") : _T(" "),
				plugin->m_name,
				sFileName,
				sModifiedTime
			);
		}
		else
		{
			sPluginText = strutils::format
			(_T("\r\n  %s%-36s"),
				plugin->m_disabled ? _T("!") : _T(" "),
				plugin->m_name
			);
		}
		m_pfile->WriteString(sPluginText);
	}
}

/**
 * @brief String wrapper around API call GetLocaleInfo
 */
static String GetLocaleString(LCID locid, LCTYPE lctype)
{
	tchar_t buffer[512];
	if (!GetLocaleInfo(locid, lctype, buffer, sizeof(buffer)/sizeof(buffer[0])))
		buffer[0] = 0;
	return buffer;
}

/**
 * @brief Write string item
 */
void CConfigLog::WriteItem(int indent, const String& key, const tchar_t *value /*= nullptr*/)
{
	String text = strutils::format(value ? _T("%*.0s%s: %s\r\n") : _T("%*.0s%s:\r\n"), indent, key, key, value);
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
	String text = strutils::format(_T("%*.0s%s: %ld\r\n"), indent, key, key, value);
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
	String path2 = path;
	if (path2.find(_T(".\\")) == 0)
	{
		// Remove "relative path" info for Win API calls.
		const tchar_t *pf = path2.c_str();
		path2 = String(pf+2);
	}
	String name = paths::FindFileName(path2);
	CVersionInfo vi(path2.c_str(), true);
	String sModifiedTime = _T("");
	if (name != path)
	{
		sModifiedTime = GetLastModified(path);
		if (!sModifiedTime.empty())
			sModifiedTime = _T("  [") + sModifiedTime + _T("]");
	}
	String text = strutils::format
	(
		name == path
			?	_T(" %*s%-18s %s=%u.%02u %s=%04u\r\n")
			:	_T(" %*s%-18s %s=%u.%02u %s=%04u path=%s%s\r\n"),
		indent,
		// Tilde prefix for modules currently mapped into WinMerge
		GetModuleHandle(path2.c_str()) 
			? _T("~") 
			: _T("")/*name*/,
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
		path,
		sModifiedTime
	);
	m_pfile->WriteString(text);
}

/**
 * @brief Write winmerge configuration
 */
void CConfigLog::WriteWinMergeConfig()
{
	TempFile tmpfile;
	String tmppath = tmpfile.Create();
	GetOptionsMgr()->ExportOptions(tmppath, true);
	UniMemFile ufile;
	if (!ufile.OpenReadOnly(tmppath))
		return;
	String line;
	bool lossy;
	while (ufile.ReadString(line, &lossy)) 
	{
		String prefix = _T("  ");
		if (line[0] == _T('[') )
			prefix = _T(" ");
		FileWriteString(prefix + line + _T("\r\n"));
	}
	ufile.Close();
}

/** 
 * @brief Write logfile
 */
bool CConfigLog::DoFile(String &sError)
{
	CVersionInfo version;
	String text;

	String sFileName = paths::ConcatPath(env::GetMyDocuments(), WinMergeDocumentsFolder);
	paths::CreateIfNeeded(sFileName);
	m_sFileName = paths::ConcatPath(sFileName, _T("WinMerge.txt"));

	if (!m_pfile->OpenCreateUtf8(m_sFileName))
	{
		m_sFileName = paths::ConcatPath(env::GetTemporaryPath(), _T("WinMerge.txt"));
		if (!m_pfile->OpenCreateUtf8(m_sFileName))
		{
			const UniFile::UniError& err = m_pfile->GetLastUniError();
			sError = err.GetError();
			return false;
		}
	}
	m_pfile->SetBom(true);
	m_pfile->WriteBom();

// Begin log
	FileWriteString(_T("WinMerge Configuration Log\r\n"));
	FileWriteString(_T("--------------------------\r\n"));
	FileWriteString(_T("\r\nLog Saved to:         "));
	FileWriteString(m_sFileName);
	FileWriteString(_T("\r\n                >> >> Please add this information (or attach this file) when reporting bugs << <<"));

// Platform stuff
	
	FileWriteString(_T("\r\n\r\nWindows Info:         "));
	text = GetWindowsVer();
	FileWriteString(text);
	text = GetProcessorInfo();
	if (!text.empty())
	{		
		FileWriteString(_T("\r\n Processor:           "));
		FileWriteString(text);
	}

// WinMerge stuff

	FileWriteString(_T("\r\n\r\nWinMerge Info:"));
	String sEXEFullFileName = paths::GetLongPath(version.GetFullFileName(), false);
	FileWriteString(_T("\r\n Code File:           "));
	FileWriteString(sEXEFullFileName);

	FileWriteString(_T("\r\n Version:             "));
	FileWriteString(version.GetProductVersion());
	String privBuild = version.GetPrivateBuild();
	if (!privBuild.empty())
	{
		FileWriteString(_T(" + ") + privBuild);
	}

	FileWriteString(_T("\r\n Code File Modified:  "));
	FileWriteString(GetLastModified(sEXEFullFileName));

	FileWriteString(_T("\r\n Build Config:       "));
	FileWriteString(GetBuildFlags());

	FileWriteString(_T("\r\n Build Software:      "));
	FileWriteString(GetCompilerVersion());

	const tchar_t* szCmdLine = ::GetCommandLine();
	assert(szCmdLine != nullptr);

	// Skip the quoted executable file name.
	if (szCmdLine != nullptr)
	{
		szCmdLine = tc::tcschr(szCmdLine, '"');
		if (szCmdLine != nullptr)
		{
			szCmdLine += 1; // skip the opening quote.
			szCmdLine = tc::tcschr(szCmdLine, '"');
			if (szCmdLine != nullptr)
			{
				szCmdLine += 1; // skip the closing quote.
			}
		}
	}

	// The command line include a space after the executable file name,
	// which mean that empty command line will have length of one.
	if (!szCmdLine || lstrlen(szCmdLine) < 2)
	{
		szCmdLine = _T(" none");
	}

	FileWriteString(_T("\r\n\r\nCommand Line:        "));
	FileWriteString(szCmdLine);

	tchar_t szCurrentDirectory[MAX_PATH]{};
	GetCurrentDirectory(sizeof(szCurrentDirectory) / sizeof(tchar_t), szCurrentDirectory);
	FileWriteString(_T("\r\n\r\nCurrent Directory:    "));
	FileWriteString(szCurrentDirectory);

	FileWriteString(_T("\r\n\r\nModule Names:         '~' prefix indicates module is loaded into the WinMerge process.\r\n"));
	FileWriteString(_T(" Windows:\r\n"));
	WriteVersionOf1(2, _T("kernel32.dll"));
	WriteVersionOf1(2, _T("shell32.dll"));
	WriteVersionOf1(2, _T("shlwapi.dll"));
	WriteVersionOf1(2, _T("COMCTL32.dll"));
	WriteVersionOf1(2, _T("msvcrt.dll"));
	FileWriteString(_T(        " WinMerge:            Path names are relative to the Code File's directory.\r\n"));
	WriteVersionOf1(2, _T(".\\ShellExtensionU.dll"));
	WriteVersionOf1(2, _T(".\\ShellExtensionX64.dll"));
	WriteVersionOf1(2, _T(".\\ShellExtensionARM.dll"));
	WriteVersionOf1(2, _T(".\\ShellExtensionARM64.dll"));
	WriteVersionOf1(2, _T(".\\WinMergeContextMenu.dll"));
	WriteVersionOf1(2, _T(".\\Frhed\\hekseditU.dll"));
	WriteVersionOf1(2, _T(".\\WinIMerge\\WinIMergeLib.dll"));
	WriteVersionOf1(2, _T(".\\WinWebDiff\\WinWebDiffLib.dll"));
	WriteVersionOf1(2, _T(".\\Merge7z\\7z.dll"));

// System settings
	FileWriteString(_T("\r\nSystem Settings:\r\n"));
	FileWriteString(_T(" Codepage Settings:\r\n"));
	WriteItem(2, _T("ANSI codepage"), GetACP());
	WriteItem(2, _T("OEM codepage"), GetOEMCP());
#ifndef UNICODE
	WriteItem(2, _T("multibyte codepage"), _getmbcp());
#endif
	WriteLocaleSettings(GetThreadLocale(), _T("Locale (Thread)"));
	WriteLocaleSettings(LOCALE_USER_DEFAULT, _T("Locale (User)"));
	WriteLocaleSettings(LOCALE_SYSTEM_DEFAULT, _T("Locale (System)"));

// Plugins
	FileWriteString(_T("\r\nPlugins:                                '!' Prefix indicates the plugin is Disabled.\r\n"));
	FileWriteString(    _T(" Unpackers:                             Path names are relative to the Code File's directory."));
	WritePluginsInLogFile(L"URL_PACK_UNPACK");
	WritePluginsInLogFile(L"FILE_PACK_UNPACK");
	WritePluginsInLogFile(L"BUFFER_PACK_UNPACK");
	WritePluginsInLogFile(L"FILE_FOLDER_PACK_UNPACK");
	FileWriteString(_T("\r\n Prediffers: "));
	WritePluginsInLogFile(L"FILE_PREDIFF");
	WritePluginsInLogFile(L"BUFFER_PREDIFF");
	FileWriteString(_T("\r\n Editor scripts: "));
	WritePluginsInLogFile(L"EDITOR_SCRIPT");
	if (!plugin::IsWindowsScriptThere())
		FileWriteString(_T("\r\n .sct scripts disabled (Windows Script Host not found)\r\n"));

	FileWriteString(_T("\r\n\r\n"));

// WinMerge settings
	FileWriteString(_T("\r\nWinMerge configuration:\r\n"));
	WriteWinMergeConfig();

	CloseFile();

	return true;
}

/** 
 * @brief Parse Windows version data to string.
 * @return String describing Windows version.
 */
String CConfigLog::GetWindowsVer()
{
	CRegKeyEx key;
	if (key.QueryRegMachine(_T("Software\\Microsoft\\Windows NT\\CurrentVersion")))
	{
		String productName = key.ReadString(_T("ProductName"), _T("Unknown OS"));
		if (HMODULE hModule = GetModuleHandle(_T("ntdll.dll")))
		{
			using RtlGetNtVersionNumbersFunc = void (WINAPI*)(DWORD*, DWORD*, DWORD*);
			DWORD dwMajor = 0, dwMinor = 0, dwBuildNumber = 0;
			if (RtlGetNtVersionNumbersFunc RtlGetNtVersionNumbers =
				reinterpret_cast<RtlGetNtVersionNumbersFunc>(GetProcAddress(hModule, "RtlGetNtVersionNumbers")))
			{
				RtlGetNtVersionNumbers(&dwMajor, &dwMinor, &dwBuildNumber);
				dwBuildNumber &= ~0xF0000000;
				if (dwMajor == 10 && dwMinor == 0 && dwBuildNumber >= 22000)
					strutils::replace(productName, _T("Windows 10"), _T("Windows 11"));
			}
		}
		return productName;
	}
	return _T("Unknown OS");
}


/** 
 * @brief Parse Processor Information data to string.
 * @return String describing Windows version.
 */
String CConfigLog::GetProcessorInfo()
{
	CRegKeyEx key;
	String sProductName = _T("");
	if (key.QueryRegMachine(_T("Hardware\\Description\\System\\CentralProcessor\\0")))
		sProductName = key.ReadString(_T("Identifier"), _T(""));
	if (!sProductName.empty())
	{
		// This is the full identifier of the processor
		//	(e.g. "Intel64 Family 6 Model 158 Stepping 9")
		//	but we'll only keep the first word (e.g. "Intel64")
		int x = (int)sProductName.find_first_of(_T(' '));
		sProductName = sProductName.substr(0, x);
	}


	// Number of processors, Amount of memory
	SYSTEM_INFO siSysInfo;
	::GetSystemInfo(&siSysInfo); 

	MEMORYSTATUSEX GlobalMemoryBuffer = {sizeof (GlobalMemoryBuffer)};
	::GlobalMemoryStatusEx(&GlobalMemoryBuffer);
	ULONG lInstalledMemory = (ULONG)(GlobalMemoryBuffer.ullTotalPhys / (1024*1024));

	tchar_t buf[MAX_PATH];
	swprintf_s(buf, MAX_PATH, _T("%u Logical Processors, %u MB Memory"), 
			siSysInfo.dwNumberOfProcessors, lInstalledMemory); 

	return sProductName + _T(", ") + String(buf);
}
	
/** 
 * @brief Return string representation of build flags (for reporting in config log)
 */
String CConfigLog::GetBuildFlags()
{
	String flags;

#if defined WIN64
	flags += _T(" WIN64 ");
#elif defined WIN32
	flags += _T(" WIN32 ");
#endif

#if defined UNICODE
	flags += _T(" UNICODE ");
#endif

#if defined _DEBUG
	flags += _T(" _DEBUG ");
#endif

#if defined TEST_WINMERGE
	flags += _T(" TEST_WINMERGE ");
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

