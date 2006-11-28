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
 * @file  CmdLineParser.h
 *
 * @brief CmdLineParser class implementation.
 *
 */

// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"

#include <shlwapi.h> // Required for PathFindFileName

#include "MainFrm.h"
#include "MergeCmdLineInfo.h"
#include "ClearCaseCmdLineParser.h"
#include "WinMergeCmdLineParser.h"

// MergeCmdLineInfo

IMPLEMENT_SERIAL(MergeCmdLineInfo, CCommandLineInfo, 1)

MergeCmdLineInfo::MergeCmdLineInfo(const TCHAR *szExeName) : CCommandLineInfo(),
	m_nCmdShow(SW_SHOWNORMAL),
	m_bClearCaseTool(false),
	m_bEscShutdown(false),
	m_bExitIfNoDiff(false),
	m_bRecurse(false),
	m_bNonInteractive(false),
	m_bNoPrefs(false),
	m_bSingleInstance(false),
	m_bShowUsage(false),
	m_dwLeftFlags(FFILEOPEN_CMDLINE),
	m_dwRightFlags(FFILEOPEN_CMDLINE),
	m_nFiles(0)
{
	m_Files.SetSize(2);

	// Rational ClearCase has a weird way of executing external
	// tools which replace the build-in ones. It also doesn't allow
	// you to define which parameters to send to the executable.
	// So, in order to run as an external tool, WinMerge should do:
	// if argv[0] is "xcompare" then it "knows" that it was
	// executed from ClearCase. In this case, it should read and
	// parse ClearCase's command line parameters and not the
	// "regular" parameters. More information can be found in
	// C:\Program Files\Rational\ClearCase\lib\mgrs\mgr_info.h file.

	LPTSTR szFileName = ::PathFindFileName(szExeName);

	if (lstrcmpi(szFileName, _T("xcompare")) && lstrcmpi(szFileName, _T("xmerge")))
	{
		m_pCmdLineParser = new WinMergeCmdLineParser(*this);
	}
	else
	{
		m_bClearCaseTool = true;
		m_pCmdLineParser = new ClearCaseCmdLineParser(*this, szFileName);		
	}
}

MergeCmdLineInfo::~MergeCmdLineInfo()
{
	delete m_pCmdLineParser;
}

void MergeCmdLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	// Give our base class a chance to figure out what is the parameter.
	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);

	m_pCmdLineParser->ParseParam(pszParam, bFlag, bLast);
}

void MergeCmdLineInfo::Serialize(CArchive& ar)
{
	CCommandLineInfo::Serialize(ar);

	if (ar.IsStoring())
	{
		Store(ar);
	}
	else
	{
		Load(ar);
	}
}

void MergeCmdLineInfo::Store(CArchive& ar)
{
	ASSERT(ar.IsStoring());

	ar << m_nCmdShow;
	ar << m_bClearCaseTool;
	ar << m_bEscShutdown;
	ar << m_bExitIfNoDiff;
	ar << m_bRecurse;
	ar << m_bNonInteractive;
	ar << m_bNoPrefs;
	ar << m_bSingleInstance;
	ar << m_bShowUsage;
	ar << m_dwLeftFlags;
	ar << m_dwRightFlags;
	ar << m_sLeftDesc;
	ar << m_sRightDesc;
	ar << m_sFileFilter;
	ar << m_sPreDiffer;

	ar << m_Settings.GetCount();

	if (m_Settings.GetCount() > 0)
	{
		CMapStringToString::CPair* pIter = m_Settings.PGetFirstAssoc();
		while (pIter != NULL)
		{
			ar << pIter->key;
			ar << pIter->value;

			pIter = m_Settings.PGetNextAssoc(pIter);
		}
	}

	ar << m_nFiles;
	for (INT_PTR i = 0; i < m_nFiles; ++i)
	{
		ar << m_Files[i];
	}
}

void MergeCmdLineInfo::Load(CArchive& ar)
{
	ASSERT(ar.IsLoading());

	INT_PTR i;

	ar >> m_nCmdShow;
	ar >> m_bClearCaseTool;
	ar >> m_bEscShutdown;
	ar >> m_bExitIfNoDiff;
	ar >> m_bRecurse;
	ar >> m_bNonInteractive;
	ar >> m_bNoPrefs;
	ar >> m_bSingleInstance;
	ar >> m_bShowUsage;
	ar >> m_dwLeftFlags;
	ar >> m_dwRightFlags;
	ar >> m_sLeftDesc;
	ar >> m_sRightDesc;
	ar >> m_sFileFilter;
	ar >> m_sPreDiffer;

	INT_PTR nSettings;
	ar >> nSettings;

	for (i = 0; i < nSettings; ++i)
	{
		CString sParam;
		CString sValue;
		ar >> sParam;
		ar >> sValue;

		m_Settings[sParam] = sValue;
	}

	ar >> m_nFiles;
	m_Files.SetSize(m_nFiles);

	for (i = 0; i < m_nFiles; ++i)
	{
		CString sFile;
		ar >> sFile;

		m_Files.SetAtGrow(i, sFile);
	}
}
