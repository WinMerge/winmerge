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
 *  @file DiffContext.cpp
 *
 *  @brief Implementation of CDiffContext
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "merge.h"
#include "version.h"
#include "DiffContext.h"
#include "paths.h"
#include "coretools.h"
#include "UniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


static void GuessEncoding(const CString & filepath, int * unicoding, int * codepage);
static bool demoGuessEncoding_html(UniFile * pufile, int * encoding, int * codepage);
static bool demoGuessEncoding_rc(UniFile * pufile, int * encoding, int * codepage);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiffContext::CDiffContext(LPCTSTR pszLeft /*=NULL*/, LPCTSTR pszRight /*=NULL*/)
{
	m_bRecurse=FALSE;
	m_strLeft = pszLeft;
	m_strRight = pszRight;
	m_pList = &m_dirlist;

	pNamesLeft = NULL;
	pNamesRight = NULL;
	m_piFilterUI = 0;
	m_piFilterGlobal = 0;
	m_msgUpdateStatus = 0;
	m_hDirFrame = NULL;
}

CDiffContext::CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight, CDiffContext& src)
{
	// This is used somehow in recursive comparisons
	// I think that it is only used during rescan to copy into temporaries
	// which modify the original (because pointers are copied below)
	// and then the temporary goes away
	// so the temporary never exists while the user is interacting with the GUI

	m_bRecurse=src.m_bRecurse;
	m_strLeft = pszLeft;
	m_strRight = pszRight;
	m_pList = src.m_pList;
	m_piFilterUI = src.m_piFilterUI;
	m_piFilterGlobal = src.m_piFilterGlobal;
	m_msgUpdateStatus = src.m_msgUpdateStatus;
	m_hDirFrame = src.m_hDirFrame;

	pNamesLeft = NULL;
	pNamesRight = NULL;
}

CDiffContext::~CDiffContext()
{
	if (pNamesLeft != NULL)
		free(pNamesLeft);
	if (pNamesRight != NULL)
		free(pNamesRight);
}


/**
 * @brief Fetch & return the fixed file version as a dotted string
 */
static CString GetFixedFileVersion(const CString & path)
{
	CVersionInfo ver(path);
	return ver.GetFixedFileVersion();
}

/**
 * @brief Add new diffitem to CDiffContext array
 */
void CDiffContext::AddDiff(const CString &pszFilename, const CString &szSubdir
	, const CString &pszLeftDir, const CString &pszRightDir
	, __int64 lmtime, __int64 rmtime
	, __int64 lctime, __int64 rctime
	, __int64 lsize, __int64 rsize
	, int diffcode
	, int lattrs, int rattrs
	, int ndiffs, int ntrivialdiffs
	)
{
	DIFFITEM di;
	di.sfilename = pszFilename;
	di.sSubdir = szSubdir;
	di.left.spath = pszLeftDir;
	di.right.spath = pszRightDir;
	di.left.mtime = lmtime;
	di.right.mtime = rmtime;
	di.left.ctime = lctime;
	di.right.ctime = rctime;
	di.diffcode = diffcode;
	di.left.size = lsize;
	di.right.size = rsize;
	UpdateFieldsNeededForNewItems(di, di.left);
	UpdateFieldsNeededForNewItems(di, di.right);
	di.left.flags.flags += lattrs;
	di.right.flags.flags += rattrs;
	if (ndiffs >= 0 && ntrivialdiffs >= 0)
	{
		di.nsdiffs = ndiffs - ntrivialdiffs;
		di.ndiffs = ndiffs;
	}
	AddDiff(di);
}

/**
 * @brief Add new diffitem to CDiffContext array
 */
void CDiffContext::AddDiff(DIFFITEM & di)
{
	// BSP - Capture the extension; from the end of the file name to the last '.'     
	TCHAR *pDest = _tcsrchr(di.sfilename, _T('.') );

	if(pDest)	// handle no extensions case.
	{
		di.sext = pDest+1; // skip dot
		di.sext.MakeLower();
	}

	m_pList->AddTail(di);
	// ignore return value
	SendMessage(m_hDirFrame, m_msgUpdateStatus, di.diffcode, NULL);
}

/**
 * @brief Remove diffitem from CDiffContext array
 * @param diffpos position of item to remove
 */
void CDiffContext::RemoveDiff(POSITION diffpos)
{
	m_pList->RemoveAt(diffpos);
}

/**
 * @brief Sets regular expression for DiffContext
 */
void CDiffContext::SetRegExp(LPCTSTR pszExp)
{
	m_strRegExp = pszExp;
}

/**
 * @brief Empty CDiffContext array
 */
void CDiffContext::RemoveAll()
{
	m_pList->RemoveAll();
}

/**
 * @brief Get position of first item in CDiffContext array
 */
POSITION CDiffContext::GetFirstDiffPosition()
{
	return m_pList->GetHeadPosition();
}

/**
 * @brief Get position of next item in CDiffContext array
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item in current position
 */
DIFFITEM CDiffContext::GetNextDiffPosition(POSITION & diffpos)
{
	return m_pList->GetNext(diffpos);
}

/**
 * @brief Get Diff Item at given position of CDiffContext array
 * @param diffpos position of item to return
 */
const DIFFITEM & CDiffContext::GetDiffAt(POSITION diffpos) const
{
	return m_pList->GetAt(diffpos);
}
DIFFITEM & CDiffContext::GetDiffAt(POSITION diffpos)
{
	return m_pList->GetAt(diffpos);
}


/**
 * @brief Get number of items in CDiffContext array
 */
int CDiffContext::GetDiffCount()
{
	return m_pList->GetCount();
}

/**
 * @brief Alter some bit flags of the diffcode.
 *
 * Examples:
 *  SetDiffStatusCode(pos, DIFFCODE::SAME, DIFFCODE::COMPAREFLAGS)
 *   changes the comparison result to be the same.
 * 
 *  SetDiffStatusCode(pos, DIFFCODE::BOTH, DIFFCODE::SIDEFLAG)
 *   changes the side status to be both (sides).
 *
 * SetDiffStatusCode(pos, DIFFCODE::SAME+DIFFCODE::BOTH, DIFFCODE::COMPAREFLAGS+DIFFCODE::SIDEFLAG);
 *  changes the comparison result to be the same and the side status to be both
 */
void CDiffContext::SetDiffStatusCode(POSITION diffpos, UINT diffcode, UINT mask)
{
	ASSERT(diffpos);
	DIFFITEM & di = m_pList->GetAt(diffpos);
	ASSERT(! ((~mask) & diffcode) ); // make sure they only set flags in their mask
	di.diffcode &= (~mask); // remove current data
	di.diffcode |= diffcode; // add new data
}

/**
 * @brief Load all fields not provided in initial AddDiff call
 */
void CDiffContext::UpdateFieldsNeededForNewItems(DIFFITEM & di, DiffFileInfo & dfi)
{
	// attributes weren't passed, which means reading the file status
	// so we may as well do them all
	UpdateInfoFromDiskHalf(di, dfi);
}

/**
 * @brief Update the diffitem passed from disk
 */
void CDiffContext::UpdateInfoFromDisk(DIFFITEM & di)
{
	UpdateInfoFromDiskHalf(di, di.left);
	UpdateInfoFromDiskHalf(di, di.right);
}

/**
 * @brief Update info in list (specified by position) from disk
 */
void CDiffContext::UpdateStatusFromDisk(POSITION diffpos)
{
	UpdateInfoFromDisk(m_pList->GetAt(diffpos));
}

/**
 * @brief Convert a FILETIME to a long (standard time)
 */
static __int64 FileTimeToInt64(FILETIME & ft)
{
	return CTime(ft).GetTime();
}

/**
 * @brief Update information from disk (for one side)
 */
void CDiffContext::UpdateInfoFromDiskHalf(DIFFITEM & di, DiffFileInfo & dfi)
{
	UpdateVersion(di, dfi);

	CString filepath = paths_ConcatPath(dfi.spath, di.sfilename);

	// CFileFind doesn't expose the attributes
	// CFileStatus doesn't expose 64 bit size

	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(filepath, &wfd);
	__int64 mtime64 = 0;
	if (h != INVALID_HANDLE_VALUE)
	{
		mtime64 = FileTimeToInt64(wfd.ftLastWriteTime);
		dfi.flags.reset();
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			dfi.flags.flags += FileFlags::RO;
		dfi.size = (wfd.nFileSizeHigh << 32) + wfd.nFileSizeLow;
		FindClose(h);
	}
	else
	{
		dfi.size = 0;
		dfi.flags.reset();
	}
	dfi.mtime = mtime64;
	GuessEncoding(filepath, &dfi.unicoding, &dfi.codepage);
}

/**
 * @brief Try to deduce encoding for this file
 */
static void
GuessEncoding(const CString & filepath, int * unicoding, int * codepage)
{
	UniMemFile ufile;
	UniFile * pufile = &ufile;

	if (!pufile->OpenReadOnly(filepath))
		return;
	bool hasbom = pufile->ReadBom();
	*unicoding = pufile->GetUnicoding();
	*codepage = pufile->GetCodepage();
	if (!hasbom)
	{
		// TODO: 
		// remove these when plugin event implemented for this
		if (!filepath.Right(4).CompareNoCase(_T(".htm"))
			|| !filepath.Right(5).CompareNoCase(_T(".html")))
		{
			demoGuessEncoding_html(pufile, unicoding, codepage);
		}
		if (!filepath.Right(3).CompareNoCase(_T(".rc")))
		{
			demoGuessEncoding_rc(pufile, unicoding, codepage);
		}
	}
}

/**
 * @brief Parser for HTML files to find encoding information
 *
 * To be removed when plugin event added for this
 */
static bool
demoGuessEncoding_html(UniFile * pufile, int * encoding, int * codepage)
{
	CString line, eol;
	while (1)
	{
		if (pufile->GetLineNumber() > 30)
			break;
		if (!pufile->ReadString(line, eol))
			break;
		static CString metapref = _T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=");
		if (line.Left(metapref.GetLength()).CompareNoCase(metapref))
			continue;

		CString cpstring = line.Mid(metapref.GetLength());
		int closequote = cpstring.Find('"');
		if (closequote == -1)
			break;
		cpstring = cpstring.Left(closequote);
		int cp=0;
		// TODO: Map ISO-8859-1 pages to codenumbers (is this possible ?)
		if (1 == _stscanf(cpstring, _T("%d"), &cp))
		{
			*codepage = cp;
			return true;
		}
	}
	return false;
}

/**
 * @brief Parser for rc files to find encoding information
 *
 * To be removed when plugin event added for this
 */
static bool
demoGuessEncoding_rc(UniFile * pufile, int * encoding, int * codepage)
{
	CString line, eol;
	while (1)
	{
		if (pufile->GetLineNumber() > 30)
			break;
		if (!pufile->ReadString(line, eol))
			break;
		int cp=0;
		if (1 == _stscanf(line, _T("#pragma code_page(%d)"), &cp)
			&& cp>0)
		{
			*codepage = cp;
			return true;
		}
	}
	return false;
}

/**
 * @brief Load file versions from disk
 */
void CDiffContext::UpdateVersion(DIFFITEM & di, DiffFileInfo & dfi)
{
	// Check only binary files
	CString filename = di.sfilename;
	filename.MakeUpper();
	if (_tcsstr(filename, _T(".EXE")) || _tcsstr(filename, _T(".DLL")) || _tcsstr(filename, _T(".SYS")) ||
	    _tcsstr(filename, _T(".DRV")) || _tcsstr(filename, _T(".OCX")) || _tcsstr(filename, _T(".CPL")) ||
	    _tcsstr(filename, _T(".SCR")))
	{
		CString filepath = paths_ConcatPath(dfi.spath, di.sfilename);
		dfi.version = GetFixedFileVersion(filepath);
	}
	else
		dfi.version = _T("");
}

/** @brief Return path to left file, including all but file name */
CString DIFFITEM::getLeftFilepath() const
{
	return left.spath;
}

/** @brief Return path to right file, including all but file name */
CString DIFFITEM::getRightFilepath() const
{
	return right.spath;
}
