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
	CString sFacename;
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

	CString GetFileName() const;
	bool WriteLogFile(CString &sError);
	void ReadLogFile(const CString & Filepath);


	// Implementation methods
private:
	bool DoFile(bool writing, CString &sError);
	void WritePluginsInLogFile(LPCWSTR transformationEvent, CStdioFile & file);
	CString GetWindowsVer();
	CString GetBuildFlags();
	void FileWriteString(LPCTSTR lpsz);
	void CloseFile();
    void WriteItemYesNo(int indent, LPCTSTR key, bool *pvalue);
	void WriteItemYesNoInverted(int indent, LPCTSTR key, bool *pvalue);
    void WriteItemYesNoInverted(int indent, LPCTSTR key, int *pvalue);
	void WriteItemWhitespace(int indent, LPCTSTR key, int *pvalue);
	bool ParseSettings(const CString & Filepath);
	CString GetValueFromConfig(const CString & key);


	// Implementation data
private:
	CString m_sFileName;
	CStdioFile m_file;
	bool m_writing;
	CfgSettings * m_pCfgSettings;
};

#endif /* _CONFIGLOG_H_ */