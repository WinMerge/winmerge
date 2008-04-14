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
 * @file  ClearCaseCmdLineParser.cpp
 *
 * @brief ClearCaseCmdLineParser class implemantation.
 *
 */

// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"

#include "ClearCaseCmdLineParser.h"
#include "MainFrm.h"
#include "MergeCmdLineInfo.h"
#include "Paths.h"

/**
 * @brief Constructor.
 * @param [out] cmdLineInfo Class which gets info about parsed parameters.
 * @param [in] szExeName Executable file name. Required in order to
 *  know which external tool was executed.
 */
ClearCaseCmdLineParser::ClearCaseCmdLineParser(MergeCmdLineInfo& CmdLineInfo,
		const TCHAR *szExeName)
: CmdLineParser(CmdLineInfo),
m_bDesc(false),
m_bBaseFile(false),
m_bOutFile(false)
{
	m_cmdLineInfo.m_bEscShutdown = true;

	m_cmdLineInfo.m_dwLeftFlags |= FFILEOPEN_READONLY | FFILEOPEN_NOMRU;
	m_cmdLineInfo.m_dwRightFlags |= FFILEOPEN_NOMRU;

	// szFileName is either "xmerge" or "xcompare".
	if (lstrcmpi(szExeName, _T("xmerge")))
	{
		m_cmdLineInfo.m_dwRightFlags |= FFILEOPEN_READONLY;

		// Compare tool doesn't have a common ancestor file description. We
		// put a phony description so the command line parser will skip it.
		m_sBaseDesc = _T("<No Base>");
	}
}

/**
 * @brief Parser function for command line parameters.
 * @param [in] pszParam The parameter or flag to parse.
 * @param [in] bFlag Is the item to parse a flag?
 * @param [in] bLast Is the item to parse a last of items?
 */
void ClearCaseCmdLineParser::ParseParam(const TCHAR* pszParam, BOOL bFlag,
		BOOL bLast)
{
	if (TRUE == bFlag)
	{
		if (!lstrcmpi(pszParam, _T("base")))
		{
			// -base is followed by common ancestor file description.
			m_bBaseFile = true;
		}
		else if (!lstrcmpi(pszParam, _T("out")))
		{
			// -out is followed by merge's output file name.
			m_bOutFile = true;
		}
		else if (!lstrcmpi(pszParam, _T("fname")))
		{
			// -fname is followed by file description.
			m_bDesc = true;
		}
	}
	else
	{
		if ((m_bBaseFile == true) && m_sBaseFile.empty())
		{
			m_sBaseFile = pszParam;
		}
		else if ((m_bOutFile == true) && m_sOutFile.empty())
		{
			m_sOutFile = pszParam;
		}
		else if ((m_bDesc == true) && m_sBaseDesc.empty())
		{
			m_sBaseDesc = pszParam;
			m_bDesc = false;
		}
		else if ((m_bDesc == true) && m_cmdLineInfo.m_sLeftDesc.IsEmpty())
		{
			m_cmdLineInfo.m_sLeftDesc = pszParam;
			m_bDesc = false;
		}
		else if ((m_bDesc == true) && m_cmdLineInfo.m_sRightDesc.IsEmpty())
		{
			m_cmdLineInfo.m_sRightDesc = pszParam;
		}
		else
		{
			String sFile = paths_GetLongPath(pszParam);
			m_cmdLineInfo.m_Files.SetAtGrow(m_cmdLineInfo.m_nFiles, sFile.c_str());
			m_cmdLineInfo.m_nFiles += 1;
		}
	}

	if (TRUE == bLast)
	{
		if (FALSE == m_sOutFile.empty())
		{
			String sFile = paths_GetLongPath(m_sOutFile.c_str());
			m_cmdLineInfo.m_Files.SetAtGrow(m_cmdLineInfo.m_nFiles, sFile.c_str());
			m_cmdLineInfo.m_nFiles += 1;
		}
	}
}
