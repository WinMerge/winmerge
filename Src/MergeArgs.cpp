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
	UINT nFiles=0;
	DWORD dwLeftFlags = FFILEOPEN_CMDLINE;
	DWORD dwRightFlags = FFILEOPEN_CMDLINE;
	BOOL recurse=FALSE;
	CString prediffer;
	CStringArray files;
	files.SetSize(2);

	// Split commandline arguments into files & flags & recursive flag

	// Rational ClearCase has a weird way of executing external
	// tools which replace the build-in ones. It also doesn't allow
	// you to define which parameters to send to the executable.
	// So, in order to run as an external tool, WinMerge should do:
	// if argv[0] is "xcompare" then it "knows" that it was
	// executed from ClearCase. In this case, it should read and
	// parse ClearCase's command line parameters and not the
	// "regular" parameters. More information can be found in
	// C:\Program Files\Rational\ClearCase\lib\mgrs\mgr_info.h file.

	if (lstrcmpi(argv[0], _T("xcompare")) && lstrcmpi(argv[0], _T("xmerge")))
	{
		ParseArgs(argc, argv, pMainFrame, files, nFiles, recurse, dwLeftFlags, dwRightFlags, prediffer);
		if (m_bShowUsage)
			return;
	}
	else
	{
		ParseCCaseArgs(argc, argv, pMainFrame, files, nFiles, dwLeftFlags, dwRightFlags);
	}

	pMainFrame->m_strSaveAsPath = _T("");

	if (nFiles > 2)
	{
		pMainFrame->m_strSaveAsPath = files[2];
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse, NULL, prediffer);
	}
	else if (nFiles > 1)
	{
		dwLeftFlags |= FFILEOPEN_CMDLINE;
		dwRightFlags |= FFILEOPEN_CMDLINE;
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse, NULL, prediffer);
	}
	else if (nFiles == 1)
	{
		CString sFilepath = files[0];
		if (IsProjectFile(sFilepath))
		{
			LoadAndOpenProjectFile(sFilepath);
		}
		else
		{
			dwRightFlags = FFILEOPEN_NONE;
			pMainFrame->DoFileOpen(sFilepath, _T(""),
				dwLeftFlags, dwRightFlags, recurse, NULL, prediffer);
		}
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

	// -x to close application if files are identical.
	if (cmdArgs.HasEmptySwitch(_T("x")))
		pMainFrame->m_bExitIfNoDiff = TRUE;

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
	int i;
	for (i = 0; i < countof(f_ArgSettings); ++i)
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

/// Process Rational ClearCase command line arguments
void
CMergeApp::ParseCCaseArgs(int argc, TCHAR *argv[], CMainFrame* pMainFrame, CStringArray & files, UINT & nFiles,
		DWORD & dwLeftFlags, DWORD & dwRightFlags)
{
	CString *pDesc = NULL;
	CString sOutFile;

	pMainFrame->m_bClearCaseTool = TRUE;
	pMainFrame->m_bEscShutdown = TRUE;

	dwLeftFlags |= FFILEOPEN_READONLY | FFILEOPEN_NOMRU;
	dwRightFlags |= FFILEOPEN_NOMRU;

	if (lstrcmpi(argv[0], _T("xmerge")))
	{
		dwRightFlags |= FFILEOPEN_READONLY;

		// First description belong to the left file.
		pDesc = &(pMainFrame->m_strLeftDesc);
	}

	for (int i = 1; i < argc; i++)
	{
		LPCTSTR pszParam = argv[i];
		if (pszParam[0] == '-')
		{
			// remove flag specifier
			++pszParam;

			if (!_tcsicmp(pszParam, _T("base")))
			{
				i++; // a 2-way merge doesn't need the common ancestor.
			}

			if (!_tcsicmp(pszParam, _T("out")))
			{
				if (i < (argc - 1))
				{
					sOutFile = argv[i+1];
					i++;
				}
			}

			// -fname "desc" - description for a file
			// Shown instead of filename
			if (!_tcsicmp(pszParam, _T("fname")))
			{
				if (i < (argc - 1))
				{
					if (NULL == pDesc)
					{
						// First description belong to the left file.
						pDesc = &(pMainFrame->m_strLeftDesc);
						
						i++; // Skip the common ancestor description.
					}
					else
					{
						LPCTSTR pszDesc = argv[i+1];
						*pDesc = pszDesc;
						i++; // Just read next parameter

						// Next description belong to the right file.
						pDesc = &(pMainFrame->m_strRightDesc);
					}
					
				}
			}
		}
		else
		{
			CString sFile = paths_GetLongPath(pszParam);
			files.SetAtGrow(nFiles, sFile);
			nFiles++;
		}
	}

	if (FALSE == sOutFile.IsEmpty())
	{
		files.SetAtGrow(nFiles, sOutFile);
		nFiles++;
	}
}

/** @brief Wrap one line of cmdline help in appropriate whitespace */
static CString CmdlineOption(int idres)
{
	CString str = LoadResString(idres) + _T(" \n");
	return str;
}

/** @brief Put together string of all cmdline arguments */
CString CMergeApp::GetUsageDescription()
{
	CString str;
	str += LoadResString(IDS_CMDLINE_SYNTAX);
	str += _T(" [/f ") + LoadResString(IDS_CMDLINE_SYNTAX_ARG_FILTER) + _T("]");
	str += _T(" ");
	str += LoadResString(IDS_CMDLINE_SYNTAX_ARGS);
	str += _T("\n\n") + LoadResString(IDS_CMDLINE_WHERE) + _T(" \n");
	str += CmdlineOption(IDS_CMDLINE_HELP);
	str += CmdlineOption(IDS_CMDLINE_RECURSIVE);
	str += CmdlineOption(IDS_CMDLINE_ESCKEY);
	str += CmdlineOption(IDS_CMDLINE_FILEMASK);
	str += CmdlineOption(IDS_CMDLINE_FASTCLOSE);
	str += CmdlineOption(IDS_CMDLINE_SINGLE_INST);
	str += CmdlineOption(IDS_CMDLINE_LEFTPATH);
	str += CmdlineOption(IDS_CMDLINE_RIGHTPATH);
	str += CmdlineOption(IDS_CMDLINE_OUTPUTPATH);
	str += _T("\n\n") + LoadResString(IDS_CMDLINE_SEEMANUAL);
	return str;
}
