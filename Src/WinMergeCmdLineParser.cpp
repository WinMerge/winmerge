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
* @file  WinMergeCmdLineParser.h
*
* @brief WinMergeCmdLineParser class implementation.
*
*/

// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"

#include "MainFrm.h"
#include "MergeCmdLineInfo.h"
#include "WinMergeCmdLineParser.h"
#include "Paths.h"


/**
 * @brief Constructor.
 * @param [out] cmdLineInfo Class which gets info about parsed parameters.
 */
WinMergeCmdLineParser::WinMergeCmdLineParser(MergeCmdLineInfo& cmdLineInfo) :
	CmdLineParser(cmdLineInfo),
	m_bPreDiff(false),
	m_bFileFilter(false),
	m_bLeftDesc(false),
	m_bRightDesc(false),
	m_bLeftDescRead(false),
	m_bRightDescRead(false)
{
}

/**
 * @brief Parser function for command line parameters.
 * @param [in] pszParam The parameter or flag to parse.
 * @param [in] bFlag Is the item to parse a flag?
 * @param [in] bLast Is the item to parse a last of items?
 */
void WinMergeCmdLineParser::ParseParam(const TCHAR* pszParam, BOOL bFlag,
		BOOL bLast)
{
	if (TRUE == bFlag)
	{
		if (!lstrcmp(pszParam, _T("?")))
		{
			// -? to show common command line arguments.
			m_cmdLineInfo.m_bShowUsage = true;
		}
		else if (!lstrcmpi(pszParam, _T("dl")))
		{
			// -dl "desc" - description for left file
			m_bLeftDesc = true;
		}
		else if (!lstrcmpi(pszParam, _T("dr")))
		{
			// -dr "desc" - description for right file
			m_bRightDesc = true;
		}
		else if (!lstrcmpi(pszParam, _T("e")))
		{
			// -e to allow closing with single esc press
			m_cmdLineInfo.m_bEscShutdown = true;
		}
		else if (!lstrcmpi(pszParam, _T("f")))
		{
			// -f "mask" - file filter mask ("*.h *.cpp")
			m_bFileFilter = true;
		}
		else if (!lstrcmpi(pszParam, _T("r")))
		{
			// -r to compare recursively
			m_cmdLineInfo.m_bRecurse = true;
		}
		else if (!lstrcmpi(pszParam, _T("s")))
		{
			// -s to allow only one instance
			m_cmdLineInfo.m_bSingleInstance = true;
		}
		else if (!lstrcmpi(pszParam, _T("noninteractive")))
		{
			// -noninteractive to suppress message boxes & close with result code
			m_cmdLineInfo.m_bNonInteractive = true;
		}
		else if (!lstrcmpi(pszParam, _T("noprefs")))
		{
			// -noprefs means do not load or remember options (preferences)
			m_cmdLineInfo.m_bNoPrefs = true;
		}
		else if (!lstrcmpi(pszParam, _T("minimize")))
		{
			// -minimize means minimize the main window.
			m_cmdLineInfo.m_nCmdShow = SW_MINIMIZE;
		}
		else if (!lstrcmpi(pszParam, _T("maximize")))
		{
			// -maximize means maximize the main window.
			m_cmdLineInfo.m_nCmdShow = SW_MAXIMIZE;
		}
		else if (!lstrcmpi(pszParam, _T("prediffer")))
		{
			// Get prediffer if specified (otherwise prediffer will be blank, which is default)
			m_bPreDiff = true;
		}
		else if (!lstrcmpi(pszParam, _T("wl")))
		{
			// -wl to open left path as read-only
			m_cmdLineInfo.m_dwLeftFlags |= FFILEOPEN_READONLY;
		}
		else if (!lstrcmpi(pszParam, _T("wr")))
		{
			// -wr to open right path as read-only
			m_cmdLineInfo.m_dwRightFlags |= FFILEOPEN_READONLY;
		}
		else if (!lstrcmpi(pszParam, _T("ul")))
		{
			// -ul to not add left path to MRU
			m_cmdLineInfo.m_dwLeftFlags |= FFILEOPEN_NOMRU;
		}
		else if (!lstrcmpi(pszParam, _T("ur")))
		{
			// -ur to not add right path to MRU
			m_cmdLineInfo.m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (!lstrcmpi(pszParam, _T("u")) || !lstrcmpi(pszParam, _T("ub")))
		{
			// -u or -ub (deprecated) to add neither right nor left path to MRU
			m_cmdLineInfo.m_dwLeftFlags |= FFILEOPEN_NOMRU;
			m_cmdLineInfo.m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (!lstrcmpi(pszParam, _T("x")))
		{
			// -x to close application if files are identical.
			m_cmdLineInfo.m_bExitIfNoDiff = true;
		}
		else
		{
			// Work around broken MFC command line parser (see another work around few
			// lines below)!
			// If the param string begins with /, even in quote marks, the parser
			// think it is a swich, not a parameter to previous switch.
			// So this works OK:
			// winmerge /dl "blaa blaa" test.txt test2.txt
			// but this does not:
			// winmerge /dl "/blaa blaa" test.txt test2.txt
			// So we use additional flags to track if left/right desc strings have
			// been read after switches for them.
			if (m_bLeftDesc && !m_bLeftDescRead)
			{
				m_cmdLineInfo.m_sLeftDesc = pszParam;
				m_bLeftDescRead = true;
				return;
			}

			if (m_bRightDesc && !m_bRightDescRead)
			{
				m_cmdLineInfo.m_sRightDesc = pszParam;
				m_bRightDescRead = true;
				return;
			}

			CString sParam = pszParam;
			CString sValue;

			int nPos = sParam.ReverseFind(_T(':'));
			if (nPos >= 0)
			{
				sParam = sParam.Left(nPos);
				sValue = sParam.Mid(nPos + 1);
			}
			else
			{
				// Treat "/ignorews" as "/ignorews:1".
				sValue = _T("1");
			}

			m_cmdLineInfo.m_Settings[sParam] = sValue;
		}
	}
	else
	{
		if ((m_bPreDiff == true) && m_cmdLineInfo.m_sPreDiffer.IsEmpty())
		{
			m_cmdLineInfo.m_sPreDiffer = pszParam;
		}
		else if ((m_bFileFilter == true) && m_cmdLineInfo.m_sFileFilter.IsEmpty())
		{
			m_cmdLineInfo.m_sFileFilter = pszParam;
		}
		else if ((m_bRightDesc == true) && m_cmdLineInfo.m_sRightDesc.IsEmpty())
		{
			m_cmdLineInfo.m_sRightDesc = pszParam;
		}
		else if ((m_bLeftDesc == true) && m_cmdLineInfo.m_sLeftDesc.IsEmpty())
		{
			m_cmdLineInfo.m_sLeftDesc = pszParam;
		}
		else
		{
			// If shortcut, expand it first
			String expanded(pszParam);
			if (paths_IsShortcut(pszParam))
			{
				expanded = ExpandShortcut(pszParam);
			}
			String sFile = paths_GetLongPath(expanded.c_str());
			m_cmdLineInfo.m_Files.SetAtGrow(m_cmdLineInfo.m_nFiles, sFile.c_str());
			m_cmdLineInfo.m_nFiles += 1;
		}
	}

	if (TRUE == bLast)
	{
		// NOTE: This is to fix a bug in MFC's parser!
		// If first path ends with "\" inside quotes (e.g. "c:\dir 1\") then
		// it is not properly splitted and second path is not separated from first
		// path. So command line like:
		// WinMerge "C:\Dir 1\" C:\Dir2
		// gets parsed as having one path! "C:\Dir 1" C:\Dir2"
		// As there are no valid quote marks in paths in this stage, we just have
		// to find quote marks and fix paths. For the second path, there may be
		// quote mark at the end - that we can just remove.
		if (m_cmdLineInfo.m_nFiles == 1)
		{
			CString file1;
			CString file2;
			int pos = m_cmdLineInfo.m_Files[0].Find(_T("\""));

			// If the quote mark found from middle of first path (not end)
			if (pos > 0 && pos < m_cmdLineInfo.m_Files[0].GetLength() - 1)
			{
				file1 = m_cmdLineInfo.m_Files[0].Left(pos);
				file2 = m_cmdLineInfo.m_Files[0].Mid(pos + 2);

				// Remove possible quotation mark from end of second path.
				// Second path got split from first path.
				if (file2[file2.GetLength() - 1] == '\"')
					file2.Delete(file2.GetLength() - 1, 1);

				// Set new paths to the structure
				m_cmdLineInfo.m_Files[0] = file1;
				m_cmdLineInfo.m_Files.SetAtGrow(1, file2);
				m_cmdLineInfo.m_nFiles = 2;
			}
		}

		// If "compare file dir" make it "compare file dir\file".
		if (m_cmdLineInfo.m_nFiles >= 2)
		{
			PATH_EXISTENCE p1 = paths_DoesPathExist(m_cmdLineInfo.m_Files[0]);
			PATH_EXISTENCE p2 = paths_DoesPathExist(m_cmdLineInfo.m_Files[1]);

			if ((p1 == IS_EXISTING_FILE) && (p2 == IS_EXISTING_DIR))
			{
				TCHAR fname[_MAX_PATH];
				TCHAR fext[_MAX_PATH];

				_tsplitpath(m_cmdLineInfo.m_Files[0], NULL, NULL, fname, fext);

				if (m_cmdLineInfo.m_Files[1].Right(1) != _T('\\'))
				{
					m_cmdLineInfo.m_Files[1] += _T('\\');
				}

				m_cmdLineInfo.m_Files[1] = m_cmdLineInfo.m_Files[1] + fname + fext;
			}
		}

		if (m_cmdLineInfo.m_bShowUsage)
		{
			m_cmdLineInfo.m_bNonInteractive = false;
		}
	}
}
