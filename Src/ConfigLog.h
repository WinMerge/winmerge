// SPDX-License-Identifier: GPL-2.0-or-later
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
	void WriteItem(int indent, const String& key, const tchar_t *value = nullptr);
	void WriteItem(int indent, const String& key, const String &str);
	void WriteItem(int indent, const String& key, long value);
	void WriteVersionOf1(int indent, const String& path);
	void WriteLocaleSettings(unsigned locid, const String& title);
	void WriteWinMergeConfig(void);

private:
	bool DoFile(String &sError);
	void WritePluginsInLogFile(const wchar_t *transformationEvent);
	static String GetWindowsVer();
	static String GetProcessorInfo();
	static String GetBuildFlags();
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
