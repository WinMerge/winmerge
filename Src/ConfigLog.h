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

	DIFFOPTIONS m_diffOptions;
	VIEWSETTINGS m_viewSettings;
	MISCSETTINGS m_miscSettings;
	CPSETTINGS m_cpSettings;
	FONTSETTINGS m_fontSettings;

	CString GetFileName() const;
	void WritePluginsInLogFile(LPCWSTR transformationEvent, CStdioFile & file);
	BOOL WriteLogFile(CString &sError);
	CString GetWindowsVer();
	CString GetBuildFlags();

private:
	CString m_sFileName;

};

#endif /* _CONFIGLOG_H_ */