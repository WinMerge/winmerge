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
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _CONFIGLOG_H_
#define _CONFIGLOG_H_

class CfgSettings;
class UniStdioFile;

/** 
 * @brief Compare-related settings.
 */
struct COMPARESETTINGS
{
	int nCompareMethod;
	bool bStopAfterFirst;
};

/** 
 * @brief View settings for directory compare
 */
struct VIEWSETTINGS
{
	bool bShowIdent;
	bool bShowDiff;
	bool bShowUniqueLeft;
	bool bShowUniqueRight;
	bool bShowBinaries;
	bool bShowSkipped;
	bool bTreeView;
};

/** 
 * @brief Misc WinMerge settings
 */
struct MISCSETTINGS
{
	bool bAutomaticRescan;
	bool bAllowMixedEol;
	bool bScrollToFirst;
	bool bBackup;
	bool bViewWhitespace;
	bool bMovedBlocks;
	bool bDetectCodepage;
	bool bShowLinenumbers;
	bool bWrapLines;
	bool bMergeMode;
	bool bSyntaxHighlight;
	int  nInsertTabs;
	int  nTabSize;
	bool bPluginsEnabled;
	bool bPreserveFiletimes;
	bool bMatchSimilarLines;
};

/** 
 * @brief Codepage WinMerge settings
 */
struct CPSETTINGS
{
	int nDefaultMode;
	int nDefaultCustomValue;
	bool bDetectCodepage;
};

/** 
 * @brief Major Font WinMerge settings
 */
struct FONTSETTINGS
{
	BYTE nCharset;
	String sFacename;
};

/** 
 * @brief Class for saving configuration log file
 */
class CConfigLog
{
public:
	CConfigLog();
	~CConfigLog();

	DIFFOPTIONS m_diffOptions;
	COMPARESETTINGS m_compareSettings;
	VIEWSETTINGS m_viewSettings;
	MISCSETTINGS m_miscSettings;
	CPSETTINGS m_cpSettings;
	FONTSETTINGS m_fontSettings;

	String GetFileName() const;
	bool WriteLogFile(String &sError);
	void ReadLogFile(const String & Filepath);

	// Implementation methods
protected:
	void WriteItem(int indent, LPCTSTR key, LPCTSTR value = 0);
	void WriteItem(int indent, LPCTSTR key, const String &str);
	void WriteItem(int indent, LPCTSTR key, long value);
	void WriteVersionOf1(int indent, LPTSTR path);
	void WriteVersionOf(int indent, LPTSTR path);
	void WriteLocaleSettings(LCID locid, LPCTSTR title);
	void WriteArchiveSupport();
	void WriteVersionOf7z(LPTSTR path);

private:
	bool DoFile(bool writing, String &sError);
	void WritePluginsInLogFile(LPCWSTR transformationEvent);
	String GetWindowsVer();
	String GetBuildFlags() const;
	void FileWriteString(LPCTSTR lpsz);
	void CloseFile();
    void WriteItemYesNo(int indent, LPCTSTR key, bool *pvalue);
	void WriteItemYesNoInverted(int indent, LPCTSTR key, bool *pvalue);
    void WriteItemYesNoInverted(int indent, LPCTSTR key, int *pvalue);
	void WriteItemWhitespace(int indent, LPCTSTR key, int *pvalue);
	bool ParseSettings(const String & Filepath);
	CString GetValueFromConfig(const CString & key);

	// Implementation data
private:
	String m_sFileName;
	UniStdioFile *m_pfile;
	bool m_writing;
	CfgSettings * m_pCfgSettings;
};

#endif /* _CONFIGLOG_H_ */