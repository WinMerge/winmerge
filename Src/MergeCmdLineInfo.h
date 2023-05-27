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
#include "FileOpenFlags.h"
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
	explicit MergeCmdLineInfo(const tchar_t *);

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

	enum WindowType
	{
		AUTOMATIC,
		TEXT,
		TABLE,
		BINARY,
		IMAGE,
		WEBPAGE,
	};

	enum DialogType
	{
		NO_DIALOG = -1,
		OPTIONS_DIALOG,
		ABOUT_DIALOG,
	};

	using usertasksflags_t = uint32_t;
	enum UserTaskFlags : usertasksflags_t
	{
		NONE                = 0x0000,
		NEW_TEXT_COMPARE    = 0x0001,
		NEW_TABLE_COMPARE   = 0x0002,
		NEW_BINARY_COMPARE  = 0x0004,
		NEW_IMAGE_COMPARE   = 0x0008,
		NEW_WEBPAGE_COMPARE = 0x0010,
		CLIPBOARD_COMPARE   = 0x1000,
		SHOW_OPTIONS_DIALOG = 0x8000,
	};

	ShowWindowType m_nCmdShow; /**< Initial state of the application's window. */
	WindowType m_nWindowType; /**< The type of window that displays the files to compare. */
	DialogType m_nDialogType; /**< The type of dialog window to display */
	bool m_bShowCompareAsMenu; /**< Show Compare As menu. */
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
	bool m_bClipboardCompare; /**< Compare text or bitmaps in the clipboard history */
	bool m_bNewCompare; /**< Show a new blank window */
	bool m_bEnableExitCode; /**< Returns the comparison result as a process exit code */
	int m_nLineIndex; /**< Line number to jump after loading files */
	int m_nCharIndex; /**< Character position to jump after loading files */
	std::optional<tchar_t> m_cTableDelimiter; /**< Delimiter character for table editing*/
	std::optional<tchar_t> m_cTableQuote; /* Quote character for table editing *< */
	std::optional<bool> m_bTableAllowNewlinesInQuotes; /**< Allow newlines in quotes */

	fileopenflags_t m_dwLeftFlags; /**< Left side file's behavior options. */
	fileopenflags_t m_dwMiddleFlags; /**< Middle side file's behavior options. */
	fileopenflags_t m_dwRightFlags; /**< Right side file's behavior options. */

	String m_sLeftDesc; /**< Left side file's description. */
	String m_sMiddleDesc; /**< Middle side file's description. */
	String m_sRightDesc; /**< Right side file's description. */

	String m_sFileFilter; /**< File filter mask. */
	String m_sPreDiffer; /**< Pre-differ name. */
	String m_sUnpacker; /**< Unpacker name. */
	String m_sFileExt; /**< File extension for determining syntax highliting */

	String m_sOutputpath;
	String m_sReportFile;

	String m_sIniFilepath;

	std::optional<usertasksflags_t> m_dwUserTasksFlags;

	PathContext m_Files; /**< Files (or directories) to compare. */

	std::map<String, String> m_Options;

	std::vector<String> m_sErrorMessages;

private:

	static const tchar_t *EatParam(const tchar_t *, String &, bool *flag = nullptr);
	const tchar_t *SetOption(const tchar_t *, const String& key, const tchar_t *value = _T("1"));
	const tchar_t *SetConfig(const tchar_t *);
	void ParseWinMergeCmdLine(const tchar_t *);
	void AddPath(const String &path);

	/** Operator= is not implemented. */
	MergeCmdLineInfo& operator=(const MergeCmdLineInfo& rhs) = delete;
};

