/////////////////////////////////////////////////////////////////////////////
//    WinMerge: An interactive diff/merge utility
//    Copyright (C) 1997 Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeCmdLineInfo.cpp
 *
 * @brief MergeCmdLineInfo class implementation.
 *
 */


#include "pch.h"
#include "MergeCmdLineInfo.h"
#include "Paths.h"
#include "OptionsDef.h"
#include "unicoder.h"

// MergeCmdLineInfo

/**
 * @brief Eat and digest a command line parameter.
 * @param [in] p Points into the command line.
 * @param [out] param Receives the digested command line parameter.
 * @param [out] flag Tells whether param is the name of a flag.
 * @return Points to the remaining portion of the command line.
 */
const tchar_t *MergeCmdLineInfo::EatParam(const tchar_t *p, String &param, bool *flag /*= nullptr*/)
{
	if (p != nullptr && *(p += tc::tcsspn(p, _T(" \t\r\n"))) == _T('\0'))
		p = nullptr;
	const tchar_t *q = p;
	if (q != nullptr)
	{
		tchar_t c = *q;
		bool quoted = false;
		do
		{
			if (c == _T('"'))
				quoted = !quoted;
			c = *++q;
		} while (c != _T('\0') && (quoted ||
			c != _T(' ') && c != _T('\t') && c != _T('\r') && c != _T('\n')));
	}
	if (q > p && flag)
	{
		if (*p == _T('-') || *p == _T('/'))
		{
			*flag = true;
			++p;
			for (const tchar_t *i = q; i >= p; --i)
				if (*i == ':')
				{
					q = i;
					break;
				}
		}
		else
		{
			*flag = false;
			flag = nullptr;
		}
	}
	param.assign(p ? p : _T(""), q - p);
	if (q > p && flag != nullptr)
	{
		param = strutils::makelower(param);
	}
	// Strip any leading or trailing whitespace or quotes
	param.erase(0, param.find_first_not_of(_T(" \t\r\n\"")));
	param.erase(param.find_last_not_of(_T(" \t\r\n\"")) + 1);
	return q;
}

/**
 * @brief Set WinMerge option from command line.
 * @param [in] p Points into the command line.
 * @param [in] key Name of WinMerge option to set.
 * @param [in] value Default value in case none is specified.
 * @return Points to the remaining portion of the command line.
 */
const tchar_t *MergeCmdLineInfo::SetOption(const tchar_t *q, const String& key, const tchar_t *value)
{
	if (!q)
		return nullptr;
	String s;
	if (*q == _T(':'))
	{
		q = EatParam(q, s);
		value = s.c_str() + 1;
	}
	m_Options.insert_or_assign(key, value);
	return q;
}

const tchar_t *MergeCmdLineInfo::SetConfig(const tchar_t *q)
{
	String s;
	if (*q == ':')
		++q;
	q = EatParam(q, s);
	size_t pos = s.find_first_of('=');
	if (pos != String::npos)
	{
		String key = s.substr(0, pos);
		String value = s.c_str() + pos + 1;
		m_Options.insert_or_assign(key, value);
	}
	return q;
}

/**
 * @brief MergeCmdLineParser's constructor.
 * @param [in] q Points to the beginning of the command line.
 */
MergeCmdLineInfo::MergeCmdLineInfo(const tchar_t* q)
	: m_nCmdShow(SHOWNORMAL)
	, m_nWindowType(AUTOMATIC)
	, m_nDialogType(NO_DIALOG)
	, m_bShowCompareAsMenu(false)
	, m_bEscShutdown(false)
	, m_bExitIfNoDiff(Disabled)
	, m_bRecurse(false)
	, m_bNonInteractive(false)
	, m_nSingleInstance()
	, m_bShowUsage(false)
	, m_bNoPrefs(false)
	, m_nCodepage(0)
	, m_bSelfCompare(false)
	, m_bClipboardCompare(false)
	, m_bNewCompare(false)
	, m_dwLeftFlags(FFILEOPEN_NONE)
	, m_dwMiddleFlags(FFILEOPEN_NONE)
	, m_dwRightFlags(FFILEOPEN_NONE)
	, m_nLineIndex(-1)
	, m_nCharIndex(-1)
	, m_bEnableExitCode(false)
{
	String exeName;
	q = EatParam(q, exeName);
	ParseWinMergeCmdLine(q);
}

/**
 * @brief Add path to list of paths.
 * This method adds given string as a path to the list of paths. Path
 * are converted if needed, shortcuts expanded etc.
 * @param [in] path Path string to add.
 */
void MergeCmdLineInfo::AddPath(const String &path)
{
	String param(path);

	// Set flag indicating path is from command line
	const size_t ord = m_Files.GetSize();
	if (ord == 0)
		m_dwLeftFlags |= FFILEOPEN_CMDLINE;
	else if (ord == 1)
		m_dwRightFlags |= FFILEOPEN_CMDLINE;
	else if (ord == 2)
		m_dwMiddleFlags |= FFILEOPEN_CMDLINE;

	if (!paths::IsURLorCLSID(path))
	{
		// Convert paths given in Linux-style ('/' as separator) given from
		// Cygwin to Windows style ('\' as separator)
		strutils::replace(param, _T("/"), _T("\\"));

		// If shortcut, expand it first
		if (paths::IsShortcut(param))
			param = paths::ExpandShortcut(param);
		param = paths::GetLongPath(param);
		m_Files.SetPath(m_Files.GetSize(), param);
	}
	else
	{
		m_Files.SetPath(m_Files.GetSize(), param, false);
	}
}

/**
 * @brief Parse native WinMerge command line.
 * @param [in] p Points into the command line.
 */
void MergeCmdLineInfo::ParseWinMergeCmdLine(const tchar_t *q)
{
	String param;
	bool flag;

	while ((q = EatParam(q, param, &flag)) != 0)
	{
		if (!flag)
		{
			// Its not a flag so it is a path
			AddPath(param);
		}
		else if (param == _T("?"))
		{
			// -? to show common command line arguments.
			m_bShowUsage = true;
		}
		else if (param == _T("o"))
		{
			// -o "outputfilename"
			q = EatParam(q, m_sOutputpath);
		}
		else if (param == _T("or"))
		{
			// -or "reportfilename"
			q = EatParam(q, m_sReportFile);
		}
		else if (param == _T("dl"))
		{
			// -dl "desc" - description for left file
			q = EatParam(q, m_sLeftDesc);
		}
		else if (param == _T("dm"))
		{
			// -dr "desc" - description for middle file
			q = EatParam(q, m_sMiddleDesc);
		}
		else if (param == _T("dr"))
		{
			// -dr "desc" - description for right file
			q = EatParam(q, m_sRightDesc);
		}
		else if (param == _T("e"))
		{
			// -e to allow closing with single esc press
			m_bEscShutdown = true;
		}
		else if (param == _T("f"))
		{
			// -f "mask" - file filter mask ("*.h *.cpp")
			q = EatParam(q, m_sFileFilter);
		}
		else if (param == _T("t"))
		{
			// -t "type" - window type
			q = EatParam(q, param);
			param = strutils::makelower(param);
			if (param == _T("automatic"))
				m_nWindowType = WindowType::AUTOMATIC;
			else if (param == _T("text"))
				m_nWindowType = WindowType::TEXT;
			else if (param == _T("table"))
				m_nWindowType = WindowType::TABLE;
			else if (param == _T("binary"))
				m_nWindowType = WindowType::BINARY;
			else if (param == _T("image"))
				m_nWindowType = WindowType::IMAGE;
			else if (param.substr(0, 3) == _T("web"))
				m_nWindowType = WindowType::WEBPAGE;
			else
				m_sErrorMessages.emplace_back(_T("Unknown window type '") + param + _T("' specified"));
		}
		else if (param == _T("show-dialog"))
		{
			// -show-dialog "type" - dialog type
			q = EatParam(q, param);
			param = strutils::makelower(param);
			if (param == _T("options"))
				m_nDialogType = DialogType::OPTIONS_DIALOG;
			else if (param == _T("about"))
				m_nDialogType = DialogType::ABOUT_DIALOG;
			else
				m_sErrorMessages.emplace_back(_T("Unknown dialog type '") + param + _T("' specified"));
		}
		else if (param == _T("show-compare-as-menu"))
		{
			m_bShowCompareAsMenu = true;
		}
		else if (param == _T("set-usertasks-to-jumplist"))
		{
			q = EatParam(q, param);
			m_dwUserTasksFlags = tc::ttoi(param.c_str());
		}
		else if (param == _T("m"))
		{
			// -m "method" - compare method
			q = EatParam(q, param);
			param = strutils::makelower(param);
			strutils::replace(param, _T("and"), _T(""));
			strutils::replace(param, _T("contents"), _T(""));
			strutils::replace(param, _T("modified"), _T(""));
			strutils::replace(param, _T(" "), _T(""));
			if (param == _T("full"))
				m_nCompMethod = CompareMethodType::CONTENT;
			else if (param == _T("quick"))
				m_nCompMethod = CompareMethodType::QUICK_CONTENT;
			else if (param == _T("binary"))
				m_nCompMethod = CompareMethodType::BINARY_CONTENT;
			else if (param == _T("date"))
				m_nCompMethod = CompareMethodType::DATE;
			else if (param == _T("sizedate") || param == _T("datesize"))
				m_nCompMethod = CompareMethodType::DATE_SIZE;
			else if (param == _T("size"))
				m_nCompMethod = CompareMethodType::SIZE;
			else
				m_sErrorMessages.emplace_back(_T("Unknown compare method '") + param + _T("' specified"));
		}
		else if (param == _T("r"))
		{
			// -r to compare recursively
			m_bRecurse = true;
		}
		else if (param == _T("s-"))
		{
			// -s- to not allow only one instance
			m_nSingleInstance = 0;
		}
		else if (param == _T("sw"))
		{
			// -sw to allow only one instance and wait for the instance to terminate
			m_nSingleInstance = 2;
		}
		else if (param == _T("s"))
		{
			// -s to allow only one instance
			if (*q == ':')
			{
				q = EatParam(q + 1, param);
				m_nSingleInstance = tc::ttoi(param.c_str());
			}
				
			else
				m_nSingleInstance = 1;
		}
		else if (param == _T("noninteractive"))
		{
			// -noninteractive to suppress message boxes & close with result code
			m_bNonInteractive = true;
		}
		else if (param == _T("noprefs"))
		{
			// -noprefs means do not load or remember options (preferences)
			m_bNoPrefs = true;
		}
		else if (param == _T("self-compare"))
		{
			// -self-compare means compare a specified file with a copy of the file
			m_bSelfCompare = true;
		}
		else if (param == _T("clipboard-compare"))
		{
			// -clipboard-compare means to compare the two most recent contents of the clipboard history.
			m_bClipboardCompare = true;
		}
		else if (param == _T("new"))
		{
			// -new means to display a new blank window
			m_bNewCompare = true;
		}
		else if (param == _T("enableexitcode"))
		{
			m_bEnableExitCode = true;
		}
		else if (param == _T("minimize"))
		{
			// -minimize means minimize the main window.
			m_nCmdShow = MINIMIZE;
		}
		else if (param == _T("maximize"))
		{
			// -maximize means maximize the main window.
			m_nCmdShow = MAXIMIZE;
		}
		else if (param == _T("unpacker"))
		{
			// Get unpacker if specified (otherwise unpacker will be blank, which is default)
			q = EatParam(q, m_sUnpacker);
		}
		else if (param == _T("prediffer"))
		{
			// Get prediffer if specified (otherwise prediffer will be blank, which is default)
			q = EatParam(q, m_sPreDiffer);
		}
		else if (param == _T("wl"))
		{
			// -wl to open left path as read-only
			m_dwLeftFlags |= FFILEOPEN_READONLY;
		}
		else if (param == _T("wm"))
		{
			// -wm to open middle path as read-only
			m_dwMiddleFlags |= FFILEOPEN_READONLY;
		}
		else if (param == _T("wr"))
		{
			// -wr to open right path as read-only
			m_dwRightFlags |= FFILEOPEN_READONLY;
		}
		else if (param == _T("ul"))
		{
			// -ul to not add left path to MRU
			m_dwLeftFlags |= FFILEOPEN_NOMRU;
		}
		else if (param == _T("um"))
		{
			// -um to not add middle path to MRU
			m_dwMiddleFlags |= FFILEOPEN_NOMRU;
		}
		else if (param == _T("ur"))
		{
			// -ur to not add right path to MRU
			m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (param == _T("u") || param == _T("ub"))
		{
			// -u or -ub (deprecated) to add neither right nor left path to MRU
			m_dwLeftFlags |= FFILEOPEN_NOMRU;
			m_dwMiddleFlags |= FFILEOPEN_NOMRU;
			m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (param == _T("fl"))
		{
			// -fl to set focus to the left panbe
			m_dwLeftFlags |= FFILEOPEN_SETFOCUS;
		}
		else if (param == _T("fm"))
		{
			// -fm to set focus to the middle pane
			m_dwMiddleFlags |= FFILEOPEN_SETFOCUS;
		}
		else if (param == _T("fr"))
		{
			// -fr to set focus to the right pane
			m_dwRightFlags |= FFILEOPEN_SETFOCUS;
		}
		else if (param == _T("l"))
		{
			// -l to set the destination line nubmer
			String line;
			q = EatParam(q, line);
			m_nLineIndex = tc::ttoi(line.c_str());
			if (m_nLineIndex <= 0)
			{
				m_nLineIndex = -1;
				m_sErrorMessages.push_back(_T("Invalid line number specified"));
			}
			else
			{
				m_nLineIndex--;
			}
		}
		else if (param == _T("c"))
		{
			// -c to set the destination character position 
			String charpos;
			q = EatParam(q, charpos);
			m_nCharIndex = tc::ttoi(charpos.c_str());
			if (m_nCharIndex <= 0)
			{
				m_nCharIndex = -1;
				m_sErrorMessages.push_back(_T("Invalid character position specified"));
			}
			else
			{
				m_nCharIndex--;
			}
		}
		else if (param == _T("table-delimiter"))
		{
			String value;
			q = EatParam(q, value);
			m_cTableDelimiter = strutils::from_charstr(value);
		}
		else if (param == _T("table-quote"))
		{
			String value;
			q = EatParam(q, value);
			m_cTableQuote = strutils::from_charstr(value);
		}
		else if (param == _T("table-allownewlinesinquotes"))
		{
			String value;
			q = EatParam(q, value);
			tchar_t c = strutils::makelower(value).c_str()[0];
			m_bTableAllowNewlinesInQuotes = (c == 0 || c == 'y' || c == 't' || c == '1');
		}
		else if (param == _T("al"))
		{
			// -al to auto-merge at the left pane
			m_dwLeftFlags |= FFILEOPEN_AUTOMERGE;
		}
		else if (param == _T("am"))
		{
			// -am to auto-merge at the middle pane
			m_dwMiddleFlags |= FFILEOPEN_AUTOMERGE;
		}
		else if (param == _T("ar"))
		{
			// -ar to auto-merge at the right pane
			m_dwRightFlags |= FFILEOPEN_AUTOMERGE;
		}
		else if (param == _T("x"))
		{
			// -x to close application if files are identical.
			m_bExitIfNoDiff = Exit;
		}
		else if (param == _T("xq"))
		{
			// -xn to close application if files are identical without showing
			// any messages
			m_bExitIfNoDiff = ExitQuiet;
		}
		else if (param == _T("cp"))
		{
			String codepage;
			q = EatParam(q, codepage);
			m_nCodepage = atoi(ucr::toUTF8(codepage).c_str());
		}
		else if (param == _T("fileext"))
		{
			q = EatParam(q, m_sFileExt);
			if (!m_sFileExt.empty() && m_sFileExt[0] != '.')
				m_sFileExt = _T(".") + m_sFileExt;
		}
		else if (param == _T("ignorews"))
		{
			q = SetOption(q, OPT_CMP_IGNORE_WHITESPACE);
		}
		else if (param == _T("ignoreblanklines"))
		{
			q = SetOption(q, OPT_CMP_IGNORE_BLANKLINES);
		}
		else if (param == _T("ignorecase"))
		{
			q = SetOption(q, OPT_CMP_IGNORE_CASE);
		}
		else if (param == _T("ignoreeol"))
		{
			q = SetOption(q, OPT_CMP_IGNORE_EOL);
		}
		else if (param == _T("ignorecodepage"))
		{
			q = SetOption(q, OPT_CMP_IGNORE_CODEPAGE);
		}
		else if (param == _T("ignorecomments"))
		{
			q = SetOption(q, OPT_CMP_FILTER_COMMENTLINES);
		}
		else if (param == _T("cfg") || param == _T("config"))
		{
			q = SetConfig(q);
		}
		else if (param == _T("inifile"))
		{
			q = EatParam(q, m_sIniFilepath);
		}
		else
		{
			m_sErrorMessages.emplace_back(_T("Unknown option '/") + param + _T("'"));
		}
	}
	// If "compare file dir" make it "compare file dir\file".
	if (m_Files.GetSize() >= 2)
	{
		paths::PATH_EXISTENCE p1 = paths::DoesPathExist(m_Files[0]);
		paths::PATH_EXISTENCE p2 = paths::DoesPathExist(m_Files[1]);

		if ((p1 == paths::IS_EXISTING_FILE) && (p2 == paths::IS_EXISTING_DIR))
		{
			m_Files[1] = paths::ConcatPath(m_Files[1], paths::FindFileName(m_Files[0]));
		}
	}
	if (m_bShowUsage)
	{
		m_bNonInteractive = false;
	}
}
