/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
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
 * @file  MergeArgs.cpp
 *
 * @brief Command line argument handling for WinMerge application
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"

#include "MainFrm.h"
#include "CmdArgs.h"
#include "paths.h"
#include "OptionsDef.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * @brief Mapping from commandline argument name (eg, ignorews) to WinMerge option name (eg, Settings/IgnoreSpace)
 *
 * These arguments take an optional colon and number, like so:
 *  "/ignoreblanklines"  (makes WinMerge ignore blank lines)
 *  "/ignoreblanklines:1"  (makes WinMerge ignore blank lines)
 *  "/ignoreblanklines:0"  (makes WinMerge not ignore blank lines)
 */
struct ArgSetting
{
	LPCTSTR CmdArgName;
	LPCTSTR WinMergeOptionName;
};
static ArgSetting f_ArgSettings[] = 
{
	{ _T("ignorews"), OPT_CMP_IGNORE_WHITESPACE }
	, { _T("ignoreblanklines"), OPT_CMP_IGNORE_BLANKLINES }
	, { _T("ignorecase"), OPT_CMP_IGNORE_CASE }
	, { _T("ignoreeol"), OPT_CMP_IGNORE_EOL }
};


/**
 * @brief Process command line arguments and open requested files/dirs if appropriate
 */
void
CMergeApp::ParseArgsAndDoOpen(int argc, TCHAR *argv[], CMainFrame* pMainFrame)
{
	CStringArray files;
	UINT nFiles=0;
	BOOL recurse=FALSE;
	files.SetSize(2);
	DWORD dwLeftFlags = FFILEOPEN_NONE;
	DWORD dwRightFlags = FFILEOPEN_NONE;
	CString prediffer;

	// Split commandline arguments into files & flags & recursive flag
	ParseArgs(argc, argv, pMainFrame, files, nFiles, recurse, dwLeftFlags, dwRightFlags, prediffer);
	if (m_bShowUsage)
		return;

	// LoadProjectFiles returns false if neither argument is a project file
	if (LoadProjectFile(files, recurse))
	{
		if (!files[0].IsEmpty())
			dwLeftFlags |= FFILEOPEN_PROJECT;
		if (!files[1].IsEmpty())
			dwRightFlags |= FFILEOPEN_PROJECT;
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse);
	}
	else if (nFiles>2)
	{
		dwLeftFlags |= FFILEOPEN_CMDLINE;
		dwRightFlags |= FFILEOPEN_CMDLINE;
		pMainFrame->m_strSaveAsPath = files[2];
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse, NULL, prediffer);
	}
	else if (nFiles>1)
	{
		dwLeftFlags |= FFILEOPEN_CMDLINE;
		dwRightFlags |= FFILEOPEN_CMDLINE;
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse, NULL, prediffer);
	}
	else if (nFiles>0)
	{
		dwLeftFlags |= FFILEOPEN_CMDLINE;
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], _T(""),
			dwLeftFlags, dwRightFlags, recurse, NULL, prediffer);
	}
}

/**
 * @brief Process all command line arguments
 */
void
CMergeApp::ParseArgs(int argc, TCHAR *argv[], CMainFrame* pMainFrame, CStringArray & files, UINT & nFiles, BOOL & recurse,
		DWORD & dwLeftFlags, DWORD & dwRightFlags, CString & prediffer)
{
	CmdArgs cmdArgs(argc, argv);

	// -? for help
	if (cmdArgs.HasEmptySwitch(_T("?")))
	{
		m_bShowUsage = true;
	}

	cmdArgs.SetCaseSensitive(false);

	// -r to compare recursively
	if (cmdArgs.HasEmptySwitch(_T("r")))
		recurse = TRUE;

	// -e to allow closing with single esc press
	if (cmdArgs.HasEmptySwitch(_T("e")))
		pMainFrame->m_bEscShutdown = TRUE;

	// -wl to open left path as read-only
	if (cmdArgs.HasEmptySwitch(_T("wl")))
		dwLeftFlags |= FFILEOPEN_READONLY;

	// -wr to open right path as read-only
	if (cmdArgs.HasEmptySwitch(_T("wr")))
		dwRightFlags |= FFILEOPEN_READONLY;

	// -ul to not add left path to MRU
	if (cmdArgs.HasEmptySwitch(_T("ul")))
		dwLeftFlags |= FFILEOPEN_NOMRU;

	// -ur to not add right path to MRU
	if (cmdArgs.HasEmptySwitch(_T("ur")))
		dwRightFlags |= FFILEOPEN_NOMRU;

	// -ub to add neither right nor left path to MRU
	if (cmdArgs.HasEmptySwitch(_T("ub")))
	{
		dwLeftFlags |= FFILEOPEN_NOMRU;
		dwRightFlags |= FFILEOPEN_NOMRU;
	}

	// -noninteractive to suppress message boxes & close with result code
	if (cmdArgs.HasEmptySwitch(_T("noninteractive")))
	{
		m_bNoninteractive = true;
	}

	// Get prediffer if specified (otherwise prediffer will be blank, which is default)
	cmdArgs.GetSwitch(_T("prediffer"), prediffer);

	if (cmdArgs.HasSwitch(_T("noprefs")))
	{
		// /noprefs means do not load or remember options (preferences)

		// turn off serializing to registry
		GetOptionsMgr()->SetSerializing(false);
		// load all default settings
		GetMainFrame()->ResetOptions();
	}

	if (cmdArgs.HasSwitch(_T("minimize")))
	{
		// /minimize means minimize the main window
		m_nCmdShow = SW_MINIMIZE;
	}

	// Handle all switches in the f_ArgSettings table
	// this are arguments mapped to WinMerge options
	int i=0;
	for (i=0; i<sizeof(f_ArgSettings)/sizeof(f_ArgSettings[0]); ++i)
	{
		const ArgSetting & argSetting = f_ArgSettings[i];
		LPCTSTR cmdargName = argSetting.CmdArgName;
		LPCTSTR optName = argSetting.WinMergeOptionName;
		CString value;
		if (cmdArgs.GetSwitch(cmdargName, value))
		{
			// eg, treat "/ignorews" as "/ignorews:1"
			if (value.IsEmpty())
				value = _T("1");
			GetOptionsMgr()->CoerceAndSaveOption(optName, value);
		}
	}

	// Can't get switches with arguments from cmdArgs
	// because cmdArgs recognizes arguments using colons not spaces

	for (i = 1; i < argc; i++)
	{
		LPCTSTR pszParam = argv[i];
		if (pszParam[0] == '-' || pszParam[0] == '/')
		{
			// remove flag specifier
			++pszParam;


			// -dl "desc" - description for left file
			// Shown instead of filename
			if (!_tcsicmp(pszParam, _T("dl")))
			{
				if (i < (argc - 1))
				{
					LPCTSTR pszDesc = argv[i+1];
					pMainFrame->m_strLeftDesc = pszDesc;
					i++;	// Just read next parameter
				}
			}

			// -dr "desc" - description for left file
			// Shown instead of filename
			if (!_tcsicmp(pszParam, _T("dr")))
			{
				if (i < (argc - 1))
				{
					LPCTSTR pszDesc = argv[i+1];
					pMainFrame->m_strRightDesc = pszDesc;
					i++;	// Just read next parameter
				}
			}

			// -f "mask" - file filter mask ("*.h *.cpp")
			if (!_tcsicmp(pszParam, _T("f")))
			{
				if (i < (argc - 1))
				{
					CString sFilter = argv[i+1];
					sFilter.TrimLeft();
					sFilter.TrimRight();
					m_globalFileFilter.SetFilter(sFilter);
					i++;	// Just read next parameter
				}
			}
		}
		else
		{
			CString sParam = pszParam;
			CString sFile = paths_GetLongPath(sParam);
			files.SetAtGrow(nFiles, sFile);
			nFiles++;
		}
	}

	// if "compare file dir" make it "compare file dir\file"
	if (nFiles >= 2)
	{
		PATH_EXISTENCE p1 = paths_DoesPathExist(files[0]);
		PATH_EXISTENCE p2 = paths_DoesPathExist(files[1]);
		if (p1 == IS_EXISTING_FILE && p2 == IS_EXISTING_DIR)
		{
			TCHAR fname[_MAX_PATH], fext[_MAX_PATH];
			_tsplitpath(files[0], NULL, NULL, fname, fext);
			if (files[1].Right(1) != _T('\\'))
				files[1] += _T('\\');
			files[1] = files[1] + fname + fext;
		}
	}

	// Reload menus in case a satellite language dll was loaded above
	ReloadMenu();
}

/** @brief Wrap one line of cmdline help in appropriate whitespace */
static CString CmdlineOption(int idres)
{
	CString str = LoadResString(idres) + _T(" \n");
	return str;
}

/** @brief Put together string of all cmdline arguments */
CString
CMergeApp::GetUsageDescription()
{

	CString str;
	str += LoadResString(IDS_CMDLINE_SYNTAX);
	str += _T(" [/f ") + LoadResString(IDS_CMDLINE_SYNTAX_ARG_FILTER) + _T("]");
	str += _T(" [/dl ") + LoadResString(IDS_CMDLINE_SYNTAX_LEFTDESC) + _T("]");
	str += _T(" [/dr ") + LoadResString(IDS_CMDLINE_SYNTAX_RIGHTDESC) + _T("]");
	str += _T(" ");
	str += LoadResString(IDS_CMDLINE_SYNTAX_ARGS);
	str += _T("\n\n") + LoadResString(IDS_CMDLINE_WHERE) + _T(" \n");
	str += CmdlineOption(IDS_CMDLINE_HELP);
	str += CmdlineOption(IDS_CMDLINE_RECURSIVE);
	str += CmdlineOption(IDS_CMDLINE_ESCKEY);
	str += CmdlineOption(IDS_CMDLINE_FILEMASK);
	str += CmdlineOption(IDS_CMDLINE_FORGETLEFT);
	str += CmdlineOption(IDS_CMDLINE_FORGETRIGHT);
	str += CmdlineOption(IDS_CMDLINE_FORGETBOTH);
	str += CmdlineOption(IDS_CMDLINE_LEFT_RO);
	str += CmdlineOption(IDS_CMDLINE_RIGHT_RO);
	str += CmdlineOption(IDS_CMDLINE_LEFT_DESC);
	str += CmdlineOption(IDS_CMDLINE_RIGHT_DESC);
	str += CmdlineOption(IDS_CMDLINE_LEFTPATH);
	str += CmdlineOption(IDS_CMDLINE_RIGHTPATH);
	str += CmdlineOption(IDS_CMDLINE_OUTPUTPATH);
	return str;
}
