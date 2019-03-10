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
 * @file  MergeCmdLineInfo.cpp
 *
 * @brief MergeCmdLineInfo class implementation.
 *
 */


#include "pch.h"
#include "MergeCmdLineInfo.h"
#include "Constants.h"
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
const TCHAR *MergeCmdLineInfo::EatParam(const TCHAR *p, String &param, bool *flag /*= nullptr*/)
{
	if (p != nullptr && *(p += _tcsspn(p, _T(" \t\r\n"))) == _T('\0'))
		p = nullptr;
	const TCHAR *q = p;
	if (q != nullptr)
	{
		TCHAR c = *q;
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
			for (const TCHAR *i = q; i >= p; --i)
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
const TCHAR *MergeCmdLineInfo::SetOption(const TCHAR *q, const String& key, const TCHAR *value)
{
	String s;
	if (*q == _T(':'))
	{
		q = EatParam(q, s);
		value = s.c_str() + 1;
	}
	m_Options.insert_or_assign(key, value);
	return q;
}

const TCHAR *MergeCmdLineInfo::SetConfig(const TCHAR *q)
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
MergeCmdLineInfo::MergeCmdLineInfo(const TCHAR *q):
	m_nCmdShow(SHOWNORMAL),
	m_bEscShutdown(false),
	m_bExitIfNoDiff(Disabled),
	m_bRecurse(false),
	m_bNonInteractive(false),
	m_bSingleInstance(false),
	m_bShowUsage(false),
	m_bNoPrefs(false),
	m_nCodepage(0),
	m_dwLeftFlags(FFILEOPEN_NONE),
	m_dwMiddleFlags(FFILEOPEN_NONE),
	m_dwRightFlags(FFILEOPEN_NONE)
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
void MergeCmdLineInfo::ParseWinMergeCmdLine(const TCHAR *q)
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
		else if (param == _T("r"))
		{
			// -r to compare recursively
			m_bRecurse = true;
		}
		else if (param == _T("s"))
		{
			// -s to allow only one instance
			m_bSingleInstance = true;
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
		else if (param == _T("cfg") || param == _T("config"))
		{
			q = SetConfig(q);
		}
		else
		{
			m_sErrorMessages.push_back(_T("Unknown option '/") + param + _T("'"));
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
