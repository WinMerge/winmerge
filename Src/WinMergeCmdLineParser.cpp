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

// RCS ID line follows -- this is updated by CVS
// $Id: WinMergeCmdLineParser.cpp $

#include "StdAfx.h"

#include "MainFrm.h"
#include "MergeCmdLineInfo.h"
#include "WinMergeCmdLineParser.h"
#include "Paths.h"

WinMergeCmdLineParser::WinMergeCmdLineParser(MergeCmdLineInfo& CmdLineInfo) :
	CmdLineParser(CmdLineInfo),
	m_bPreDiff(false),
	m_bFileFilter(false),
	m_bLeftDesc(false),
	m_bRightDesc(false)
{

}

void WinMergeCmdLineParser::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if (TRUE == bFlag)
	{
		if (!lstrcmp(pszParam, _T("?")))
		{
			// -? to show common command line arguments.
			m_CmdLineInfo.m_bShowUsage = true;
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
			m_CmdLineInfo.m_bEscShutdown = true;
		}
		else if (!lstrcmpi(pszParam, _T("f")))
		{
			// -f "mask" - file filter mask ("*.h *.cpp")
			m_bFileFilter = true;
		}
		else if (!lstrcmpi(pszParam, _T("r")))
		{
			// -r to compare recursively
			m_CmdLineInfo.m_bRecurse = true;
		}
		else if (!lstrcmpi(pszParam, _T("s")))
		{
			// -s to allow only one instance
			m_CmdLineInfo.m_bSingleInstance = true;
		}
		else if (!lstrcmpi(pszParam, _T("noninteractive")))
		{
			// -noninteractive to suppress message boxes & close with result code
			m_CmdLineInfo.m_bNonInteractive = true;
		}
		else if (!lstrcmpi(pszParam, _T("noprefs")))
		{
			// -noprefs means do not load or remember options (preferences)
			m_CmdLineInfo.m_bNoPrefs = true;
		}
		else if (!lstrcmpi(pszParam, _T("minimize")))
		{
			// -minimize means minimize the main window.
			m_CmdLineInfo.m_nCmdShow = SW_MINIMIZE;
		}
		else if (!lstrcmpi(pszParam, _T("maximize")))
		{
			// -maximize means maximize the main window.
			m_CmdLineInfo.m_nCmdShow = SW_MAXIMIZE;
		}
		else if (!lstrcmpi(pszParam, _T("prediffer")))
		{
			// Get prediffer if specified (otherwise prediffer will be blank, which is default)
			m_bPreDiff = true;
		}
		else if (!lstrcmpi(pszParam, _T("wl")))
		{
			// -wl to open left path as read-only
			m_CmdLineInfo.m_dwLeftFlags |= FFILEOPEN_READONLY;
		}
		else if (!lstrcmpi(pszParam, _T("wr")))
		{
			// -wr to open right path as read-only
			m_CmdLineInfo.m_dwRightFlags |= FFILEOPEN_READONLY;
		}
		else if (!lstrcmpi(pszParam, _T("ul")))
		{
			// -ul to not add left path to MRU
			m_CmdLineInfo.m_dwLeftFlags |= FFILEOPEN_NOMRU;
		}
		else if (!lstrcmpi(pszParam, _T("ur")))
		{
			// -ur to not add right path to MRU
			m_CmdLineInfo.m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (!lstrcmpi(pszParam, _T("ub")))
		{
			// -ub to add neither right nor left path to MRU
			m_CmdLineInfo.m_dwLeftFlags |= FFILEOPEN_NOMRU;
			m_CmdLineInfo.m_dwRightFlags |= FFILEOPEN_NOMRU;
		}
		else if (!lstrcmpi(pszParam, _T("x")))
		{
			// -x to close application if files are identical.
			m_CmdLineInfo.m_bExitIfNoDiff = true;
		}
		else
		{
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

			m_CmdLineInfo.m_Settings[sParam] = sValue;
		}
	}
	else
	{
		if ((m_bPreDiff == true) && m_CmdLineInfo.m_sPreDiffer.IsEmpty())
		{
			m_CmdLineInfo.m_sPreDiffer = pszParam;
		}
		else if ((m_bFileFilter == true) && m_CmdLineInfo.m_sFileFilter.IsEmpty())
		{
			m_CmdLineInfo.m_sFileFilter = pszParam;
		}
		else if ((m_bRightDesc == true) && m_CmdLineInfo.m_sRightDesc.IsEmpty())
		{
			m_CmdLineInfo.m_sRightDesc = pszParam;
		}
		else if ((m_bLeftDesc == true) && m_CmdLineInfo.m_sLeftDesc.IsEmpty())
		{
			m_CmdLineInfo.m_sLeftDesc = pszParam;
		}
		else
		{
			CString sFile = paths_GetLongPath(pszParam);
			m_CmdLineInfo.m_Files.SetAtGrow(m_CmdLineInfo.m_nFiles, sFile);
			m_CmdLineInfo.m_nFiles += 1;
		}
	}

	if (TRUE == bLast)
	{
		// If "compare file dir" make it "compare file dir\file".
		if (m_CmdLineInfo.m_nFiles >= 2)
		{
			PATH_EXISTENCE p1 = paths_DoesPathExist(m_CmdLineInfo.m_Files[0]);
			PATH_EXISTENCE p2 = paths_DoesPathExist(m_CmdLineInfo.m_Files[1]);

			if ((p1 == IS_EXISTING_FILE) && (p2 == IS_EXISTING_DIR))
			{
				TCHAR fname[_MAX_PATH];
				TCHAR fext[_MAX_PATH];

				_tsplitpath(m_CmdLineInfo.m_Files[0], NULL, NULL, fname, fext);

				if (m_CmdLineInfo.m_Files[1].Right(1) != _T('\\'))
				{
					m_CmdLineInfo.m_Files[1] += _T('\\');
				}

				m_CmdLineInfo.m_Files[1] = m_CmdLineInfo.m_Files[1] + fname + fext;
			}
		}

		if (m_CmdLineInfo.m_bShowUsage)
		{
			m_CmdLineInfo.m_bNonInteractive = false;
		}
	}
}
