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
// $Id: MergeCmdLineInfo.h 6940 2009-08-01 17:29:01Z kimmov $

#pragma once

#include "UnicodeString.h"
#include "PathContext.h"
#include <map>

/** 
 * @brief WinMerge's command line handler.
 * This class calls command line parser(s) and allows reading parsed values
 * from public member variables.
 */
class MergeCmdLineInfo
{
public:
	explicit MergeCmdLineInfo(const TCHAR *);

public:

	enum ExitNoDiff
	{
		Disabled, /**< Don't exit. */
		Exit, /**< Exit and show message. */
		ExitQuiet, /**< Exit and don't show message. */
	};

	enum ShowWindowType
	{
		SHOWNORMAL = 1,
		MAXIMIZE = 3,
		MINIMIZE = 6,
	};

	ShowWindowType m_nCmdShow; /**< Initial state of the application's window. */

	bool m_bEscShutdown; /**< Pressing ESC will close the application */
	ExitNoDiff m_bExitIfNoDiff; /**< Exit if files are identical. */
	bool m_bRecurse; /**< Include sub folder in directories compare. */
	bool m_bNonInteractive; /**< Suppress user's notifications. */
	bool m_bSingleInstance; /**< Allow only one instance of WinMerge executable. */
	bool m_bShowUsage; /**< Show a brief reminder to command line arguments. */
	int  m_nCodepage;  /**< Codepage. */
	bool m_bNoPrefs; /**< Do not load or remember options (preferences) */   

	unsigned m_dwLeftFlags; /**< Left side file's behavior options. */
	unsigned m_dwMiddleFlags; /**< Middle side file's behavior options. */
	unsigned m_dwRightFlags; /**< Right side file's behavior options. */

	String m_sLeftDesc; /**< Left side file's description. */
	String m_sMiddleDesc; /**< Middle side file's description. */
	String m_sRightDesc; /**< Right side file's description. */

	String m_sFileFilter; /**< File filter mask. */
	String m_sPreDiffer; /**< Pre-differ name. */

	String m_sOutputpath;
	String m_sReportFile;

	PathContext m_Files; /**< Files (or directories) to compare. */

	std::map<String, String> m_Options;

	std::vector<String> m_sErrorMessages;

private:

	static const TCHAR *EatParam(const TCHAR *, String &, bool *flag = nullptr);
	const TCHAR *SetOption(const TCHAR *, const String& key, const TCHAR *value = _T("1"));
	const TCHAR *SetConfig(const TCHAR *);
	void ParseWinMergeCmdLine(const TCHAR *);
	void AddPath(const String &path);

	/** Operator= is not implemented. */
	MergeCmdLineInfo& operator=(const MergeCmdLineInfo& rhs);
};

