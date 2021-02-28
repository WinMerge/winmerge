/////////////////////////////////////////////////////////////////////////////
//    WinMerge: An interactive diff/merge utility
//    Copyright (C) 1997 Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeCmdLineInfo.h
 *
 * @brief MergeCmdLineInfo class declaration.
 *
 */
#pragma once

#include "UnicodeString.h"
#include "PathContext.h"
#include <map>
#include <optional>

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

	enum CompareMethodType
	{
		CONTENT,
		QUICK_CONTENT,
		BINARY_CONTENT,
		DATE,
		DATE_SIZE,
		SIZE,
	};

	ShowWindowType m_nCmdShow; /**< Initial state of the application's window. */

	bool m_bEscShutdown; /**< Pressing ESC will close the application */
	ExitNoDiff m_bExitIfNoDiff; /**< Exit if files are identical. */
	bool m_bRecurse; /**< Include sub folder in directories compare. */
	std::optional<CompareMethodType> m_nCompMethod; /**< Compare method */
	bool m_bNonInteractive; /**< Suppress user's notifications. */
	std::optional<int> m_nSingleInstance; /**< Allow only one instance of WinMerge executable. */
	bool m_bShowUsage; /**< Show a brief reminder to command line arguments. */
	int  m_nCodepage;  /**< Codepage. */
	bool m_bNoPrefs; /**< Do not load or remember options (preferences) */   
	bool m_bSelfCompare; /**< Compares the specified file with a copy of the file */

	unsigned m_dwLeftFlags; /**< Left side file's behavior options. */
	unsigned m_dwMiddleFlags; /**< Middle side file's behavior options. */
	unsigned m_dwRightFlags; /**< Right side file's behavior options. */

	String m_sLeftDesc; /**< Left side file's description. */
	String m_sMiddleDesc; /**< Middle side file's description. */
	String m_sRightDesc; /**< Right side file's description. */

	String m_sFileFilter; /**< File filter mask. */
	String m_sPreDiffer; /**< Pre-differ name. */
	String m_sUnpacker; /**< Unpacker name. */

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

