/////////////////////////////////////////////////////////////////////////////
//
//    WinMerge: An interactive diff/merge utility
//    Copyright (C) 1997 Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _MERGE_CMD_LINE_INFO_INCLUDED_
#define _MERGE_CMD_LINE_INFO_INCLUDED_

/** 
 * @file  MergeCmdLineInfo.h
 *
 * @brief MergeCmdLineInfo class declaration.
 *
 */

// RCS ID line follows -- this is updated by CVS
// $Id: MergeCmdLineInfo.h $

class CmdLineParser;

/** 
 * @brief WinMerge's command line handler.
 *
 */
class MergeCmdLineInfo : public CCommandLineInfo
{
public:

	/** @brief ClearCaseCmdLineParser's constructor.
	 *
	 * @param [in] szFileName Executable file name. Required in order to
	 *	know which command line parser to create and use.
	 *
	 */
	MergeCmdLineInfo(const TCHAR *szExeName);

	~MergeCmdLineInfo();

	/** @brief Override CCommandLineInfo's method. */
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

public:

	int m_nCmdShow; /**< Initial state of the application's window. */

	bool m_bClearCaseTool; /**< Running as Rational ClearCase external tool. */
	bool m_bEscShutdown; /**< Pressing ESC will close the application */
	bool m_bExitIfNoDiff; /**< Exit after telling the user that files are identical. */
	bool m_bRecurse; /**< Include sub folder in directories compare. */
	bool m_bNonInteractive; /**< Suppress user's notifications. */
	bool m_bNoPrefs; /**< Do not load or remember preferences. */
	bool m_bSingleInstance; /**< Allow only one instance of WinMerge executable. */
	bool m_bShowUsage; /**< Show a brief reminder to command line arguments. */

	DWORD m_dwLeftFlags; /**< Left side file's behavior options. */
	DWORD m_dwRightFlags; /**< Right side file's behavior options. */

	CString m_sLeftDesc; /**< Left side file's description. */
	CString m_sRightDesc; /**< Right side file's description. */

	CString m_sFileFilter; /**< File filter mask. */
	CString m_sPreDiffer;

	/**< Command line arguments which are mapped to WinMerge's preferences. */
	CMapStringToString m_Settings;

	CStringArray m_Files; /**< Files (or directories) to compare. */

	int m_nFiles; /**< Number of files (or directories) in m_Files. */

private:

	/**< operator= is not implemented. */
	MergeCmdLineInfo& operator=(const MergeCmdLineInfo& rhs);

	CmdLineParser *m_pCmdLineParser; /**< The command line parser instance. */
};

#endif // _MERGE_CMD_LINE_INFO_INCLUDED_
