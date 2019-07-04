/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ConfigLog.h
 *
 * @brief Declaration file ConfigLog class
 */
#pragma once


#include "UnicodeString.h"
#include <memory>

class UniStdioFile;

/** 
 * @brief Class for saving configuration log file
 */
class CConfigLog
{
public:
	CConfigLog();
	~CConfigLog();

	String GetFileName() const;
	bool WriteLogFile(String &sError);

	// Implementation methods
protected:
	void WriteItem(int indent, const String& key, const TCHAR *value = nullptr);
	void WriteItem(int indent, const String& key, const String &str);
	void WriteItem(int indent, const String& key, long value);
	void WriteVersionOf1(int indent, const String& path);
	void WriteLocaleSettings(unsigned locid, const String& title);
	void WriteWinMergeConfig(void);

private:
	bool DoFile(String &sError);
	void WritePluginsInLogFile(const wchar_t *transformationEvent);
	String GetWindowsVer() const;
	String GetProcessorInfo() const;
	String GetBuildFlags() const;
	void FileWriteString(const String& lpsz);
	void CloseFile();

	// Implementation data
private:
	String m_sFileName;
	std::unique_ptr<UniStdioFile> m_pfile;
};

/**
 * @brief Return logfile name and path
 */
inline String CConfigLog::GetFileName() const
{
	return m_sFileName;
}
