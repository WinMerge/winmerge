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

#include "ConfigLog.h"
#include <cassert>
#include <windows.h>
#include <mbctype.h>
#include <memory>
#include "Constants.h"
#include "version.h"
#include "UniFile.h"
#include "Plugins.h"
#include "paths.h"
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
 * @brief Return logfile name and path
 */
String CConfigLog::GetFileName() const
{
	return m_sFileName;
}

/** 
 * @brief Write plugin names
 */
void CConfigLog::WritePluginsInLogFile(const wchar_t *transformationEvent)
{
	// get an array with the available scripts
	PluginArray * piPluginArray; 

	piPluginArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(transformationEvent);

	for (size_t iPlugin = 0 ; iPlugin < piPluginArray->size() ; iPlugin++)
	{
		const PluginInfoPtr& plugin = piPluginArray->at(iPlugin);
		m_pfile->WriteString(_T("\r\n  "));
		if (plugin->m_disabled)
			m_pfile->WriteString(_T("!"));
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
void CConfigLog::WriteItem(int indent, const String& key, const TCHAR *value)
{
	String text = strutils::format(value ? _T("%*.0s%s: %s\r\n") : _T("%*.0s%s:\r\n"), indent, key.c_str(), key.c_str(), value);
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
	String text = strutils::format(_T("%*.0s%s: %ld\r\n"), indent, key.c_str(), key.c_str(), value);
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
	String name = paths::FindFileName(path);
	CVersionInfo vi(path.c_str(), TRUE);
	String text = strutils::format
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

	String sFileName = paths::ConcatPath(env::GetMyDocuments(), WinMergeDocumentsFolder);
	paths::CreateIfNeeded(sFileName);
	m_sFileName = paths::ConcatPath(sFileName, _T("WinMerge.txt"));

	if (!m_pfile->OpenCreateUtf8(m_sFileName))
	{
		const UniFile::UniError &err = m_pfile->GetLastUniError();
		sError = err.GetError();
		return false;
	}
	m_pfile->SetBom(true);
	m_pfile->WriteBom();

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
	WriteVersionOf1(1, _T("ShellExtensionU.dll"));
	WriteVersionOf1(1, _T("ShellExtensionX64.dll"));
	WriteVersionOf1(1, _T("Frhed\\hekseditU.dll"));
	WriteVersionOf1(1, _T("WinIMerge\\WinIMergeLib.dll"));
	WriteVersionOf1(1, _T("Merge7z\\7z.dll"));

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
	WritePluginsInLogFile(L"FILE_FOLDER_PACK_UNPACK");
	FileWriteString(_T("\r\n Prediffers: "));
	WritePluginsInLogFile(L"FILE_PREDIFF");
	WritePluginsInLogFile(L"BUFFER_PREDIFF");
	FileWriteString(_T("\r\n Editor scripts: "));
	WritePluginsInLogFile(L"EDITOR_SCRIPT");
	if (plugin::IsWindowsScriptThere() == FALSE)
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
String CConfigLog::GetWindowsVer() const
{
	CRegKeyEx key;
	if (key.QueryRegMachine(_T("Software\\Microsoft\\Windows NT\\CurrentVersion")))
		return key.ReadString(_T("ProductName"), _T("Unknown OS"));
	return _T("Unknown OS");
}

/** 
 * @brief Return string representation of build flags (for reporting in config log)
 */
String CConfigLog::GetBuildFlags() const
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

