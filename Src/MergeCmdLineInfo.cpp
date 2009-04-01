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

// ID line follows -- this is updated by SVN
// $Id$

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h> // Required for PathFindFileName
#include "Constants.h"
#include "Paths.h"
#include "MergeCmdLineInfo.h"
#include "OptionsDef.h"

// MergeCmdLineInfo

/**
 * @brief Eat and digest a command line parameter.
 * @param [in] p Points into the command line.
 * @param [out] param Receives the digested command line parameter.
 * @param [out] flag Tells whether param is the name of a flag.
 * @return Points to the remaining portion of the command line.
 */
LPCTSTR MergeCmdLineInfo::EatParam(LPCTSTR p, String &param, bool *flag)
{
	if (p && *(p += StrSpn(p, _T(" \t\r\n"))) == '\0')
		p = 0;
	LPCTSTR q = PathGetArgs(p);
	if (q > p && flag)
	{
		if (*p == _T('-') || *p == _T('/'))
		{
			*flag = true;
			++p;
			if (LPCTSTR colon = StrRChr(p, q, _T(':')))
				q = colon;
		}
		else
		{
			*flag = false;
			flag = 0;
		}
	}
	param.assign(p ? p : _T(""), q - p);
	if (q > p && flag)
	{
		CharLower(&*param.begin());
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
LPCTSTR MergeCmdLineInfo::SetOption(LPCTSTR q, LPCTSTR key, LPCTSTR value)
{
	String s;
	if (*q == _T(':'))
	{
		q = EatParam(q, s);
		value = s.c_str() + 1;
	}
//	GetOptionsMgr()->SaveOption(key, value);
	return q;
}

/**
 * @brief ClearCaseCmdLineParser's constructor.
 * @param [in] q Points to the beginning of the command line.
 */
MergeCmdLineInfo::MergeCmdLineInfo(LPCTSTR q):
	m_nCmdShow(SW_SHOWNORMAL),
	m_bClearCaseTool(false),
	m_bEscShutdown(false),
	m_bExitIfNoDiff(false),
	m_bRecurse(false),
	m_bNonInteractive(false),
	m_bSingleInstance(false),
	m_bShowUsage(false),
	m_dwLeftFlags(FFILEOPEN_NONE),
	m_dwRightFlags(FFILEOPEN_NONE)
{
	m_Files.reserve(2);

	// Rational ClearCase has a weird way of executing external
	// tools which replace the build-in ones. It also doesn't allow
	// you to define which parameters to send to the executable.
	// So, in order to run as an external tool, WinMerge should do:
	// if argv[0] is "xcompare" then it "knows" that it was
	// executed from ClearCase. In this case, it should read and
	// parse ClearCase's command line parameters and not the
	// "regular" parameters. More information can be found in
	// C:\Program Files\Rational\ClearCase\lib\mgrs\mgr_info.h file.
	String exeName;
	q = EatParam(q, exeName);
	LPTSTR szFileName = PathFindFileName(exeName.c_str());
	if (lstrcmpi(szFileName, _T("xcompare")) == 0)
	{
		m_dwRightFlags |= FFILEOPEN_READONLY;
		ParseClearCaseCmdLine(q);
	}
	else if (lstrcmpi(szFileName, _T("xmerge")) == 0)
	{
		ParseClearCaseCmdLine(q);
	}
	else
	{
		ParseWinMergeCmdLine(q);
	}
}

/**
 * @brief Parse a command line passed in from ClearCase.
 * @param [in] p Points into the command line.
 */
void MergeCmdLineInfo::ParseClearCaseCmdLine(LPCTSTR q)
{
	String sBaseFile;  /**< Base file path. */
	String sBaseDesc;  /**< Base file description. */
	String sOutFile;   /**< Out file path. */
	if (m_dwRightFlags & FFILEOPEN_READONLY)
	{
		// Compare tool doesn't have a common ancestor file description. We
		// put a phony description so the command line parser will skip it.
		sBaseDesc = _T("<No Base>");
	}
	m_bClearCaseTool = true;
	String param;
	bool flag;
	while ((q = EatParam(q, param, &flag)) != 0)
	{
		if (!flag)
		{
			// Not a flag
			param = paths_GetLongPath(param.c_str());
			m_Files.push_back(param);
		}
		else if (param == _T("base"))
		{
			// -base is followed by common ancestor file description.
			q = EatParam(q, sBaseFile);
		}
		else if (param == _T("out"))
		{
			// -out is followed by merge's output file name.
			q = EatParam(q, sOutFile);
		}
		else if (param == _T("fname"))
		{
			// -fname is followed by file description.
			if (sBaseDesc.empty())
				q = EatParam(q, sBaseDesc);
			else if (m_sLeftDesc.empty())
				q = EatParam(q, m_sLeftDesc);
			else if (m_sRightDesc.empty())
				q = EatParam(q, m_sRightDesc);
			else
				q = EatParam(q, param); // ignore excess arguments
		}
	}
	if (!sOutFile.empty())
	{
		String path = paths_GetLongPath(sOutFile.c_str());
		m_Files.push_back(path);
	}
}

/**
 * @brief Parse native WinMerge command line.
 * @param [in] p Points into the command line.
 */
void MergeCmdLineInfo::ParseWinMergeCmdLine(LPCTSTR q)
{
	String param;
	bool flag;

	while ((q = EatParam(q, param, &flag)) != 0)
	{
		if (!flag)
		{
			// Its not a flag so it is a path
			// Convert paths given in Linux-style ('/' as separator) given from
			// Cygwin to Windows style ('\' as separator)
			string_replace(param, _T("/"), _T("\\"));

			// If shortcut, expand it first
			if (paths_IsShortcut(param.c_str()))
				param = ExpandShortcut(param.c_str());
			param = paths_GetLongPath(param.c_str());
			m_Files.push_back(param);
		}
		else if (param == _T("?"))
		{
			// -? to show common command line arguments.
			m_bShowUsage = true;
		}
		else if (param == _T("dl"))
		{
			// -dl "desc" - description for left file
			q = EatParam(q, m_sLeftDesc);
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
			// Turn off serializing to registry.
//			GetOptionsMgr()->SetSerializing(false);
			// Load all default settings.
//			theApp.ResetOptions();
		}
		else if (param == _T("minimize"))
		{
			// -minimize means minimize the main window.
			m_nCmdShow = SW_MINIMIZE;
		}
		else if (param == _T("maximize"))
		{
			// -maximize means maximize the main window.
			m_nCmdShow = SW_MAXIMIZE;
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
		else if (param == _T("ur"))
		{
			// -ur to not add right path to MRU
			m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (param == _T("u") || param == _T("ub"))
		{
			// -u or -ub (deprecated) to add neither right nor left path to MRU
			m_dwLeftFlags |= FFILEOPEN_NOMRU;
			m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (param == _T("x"))
		{
			// -x to close application if files are identical.
			m_bExitIfNoDiff = true;
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
	}
	// If "compare file dir" make it "compare file dir\file".
	if (m_Files.size() >= 2)
	{
		PATH_EXISTENCE p1 = paths_DoesPathExist(m_Files[0].c_str());
		PATH_EXISTENCE p2 = paths_DoesPathExist(m_Files[1].c_str());

		if ((p1 == IS_EXISTING_FILE) && (p2 == IS_EXISTING_DIR))
		{
			m_Files[1] = paths_ConcatPath(m_Files[1], paths_FindFileName(m_Files[0].c_str()));
		}
	}
	if (m_bShowUsage)
	{
		m_bNonInteractive = false;
	}
}
