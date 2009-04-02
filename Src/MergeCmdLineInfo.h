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
/** 
 * @file  MergeCmdLineInfo.h
 *
 * @brief MergeCmdLineInfo class declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _MERGE_CMD_LINE_INFO_INCLUDED_
#define _MERGE_CMD_LINE_INFO_INCLUDED_

#include <vector>
#include "UnicodeString.h"

/** 
 * @brief WinMerge's command line handler.
 * This class calls command line parser(s) and allows reading parsed values
 * from public member variables.
 */
class MergeCmdLineInfo
{
public:
	MergeCmdLineInfo(LPCTSTR);

public:

	int m_nCmdShow; /**< Initial state of the application's window. */

	bool m_bClearCaseTool; /**< Running as Rational ClearCase external tool. */
	bool m_bEscShutdown; /**< Pressing ESC will close the application */
	bool m_bExitIfNoDiff; /**< Exit after telling the user that files are identical. */
	bool m_bRecurse; /**< Include sub folder in directories compare. */
	bool m_bNonInteractive; /**< Suppress user's notifications. */
	bool m_bSingleInstance; /**< Allow only one instance of WinMerge executable. */
	bool m_bShowUsage; /**< Show a brief reminder to command line arguments. */

	DWORD m_dwLeftFlags; /**< Left side file's behavior options. */
	DWORD m_dwRightFlags; /**< Right side file's behavior options. */

	String m_sLeftDesc; /**< Left side file's description. */
	String m_sRightDesc; /**< Right side file's description. */

	String m_sFileFilter; /**< File filter mask. */
	String m_sPreDiffer; /**< Pre-differ name. */

	std::vector<String> m_Files; /**< Files (or directories) to compare. */

private:

	static LPCTSTR EatParam(LPCTSTR, String &, bool *flag = 0);
	static LPCTSTR SetOption(LPCTSTR, LPCTSTR key, LPCTSTR value = _T("1"));
	void ParseClearCaseCmdLine(LPCTSTR);
	void ParseWinMergeCmdLine(LPCTSTR);
	void AddPath(const String &path);

	/** Operator= is not implemented. */
	MergeCmdLineInfo& operator=(const MergeCmdLineInfo& rhs);
};

#endif // _MERGE_CMD_LINE_INFO_INCLUDED_
