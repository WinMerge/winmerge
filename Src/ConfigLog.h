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
// RCS ID line follows -- this is updated by CVS
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
	BOOL bStopAfterFirst;
};

/** 
 * @brief View settings for directory compare
 */
struct VIEWSETTINGS
{
	BOOL bShowIdent;
	BOOL bShowDiff;
	BOOL bShowUniqueLeft;
	BOOL bShowUniqueRight;
	BOOL bShowBinaries;
	BOOL bShowSkipped;
};

/** 
 * @brief Misc WinMerge settings
 */
struct MISCSETTINGS
{
	BOOL bAutomaticRescan;
	BOOL bAllowMixedEol;
	BOOL bScrollToFirst;
	BOOL bBackup;
	BOOL bViewWhitespace;
	BOOL bMovedBlocks;
	BOOL bDetectCodepage;
	BOOL bShowLinenumbers;
	BOOL bWrapLines;
	BOOL bMergeMode;
	BOOL bSyntaxHighlight;
	BOOL bInsertTabs;
	INT  nTabSize;
};

/** 
 * @brief Codepage WinMerge settings
 */
struct CPSETTINGS
{
	int nDefaultMode;
	int nDefaultCustomValue;
	BOOL bDetectCodepage;
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
	BOOL WriteLogFile(CString &sError);
	void ReadLogFile(const CString & Filepath);


	// Implementation methods
private:
	BOOL DoFile(bool writing, CString &sError);
	void WritePluginsInLogFile(LPCWSTR transformationEvent, CStdioFile & file);
	CString GetWindowsVer();
	CString GetBuildFlags();
	void FileWriteString(LPCTSTR lpsz);
	void CloseFile();
	void WriteItemYesNo(int indent, LPCTSTR key, BOOL *pvalue);
	void WriteItemYesNoInverted(int indent, LPCTSTR key, BOOL *pvalue);
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