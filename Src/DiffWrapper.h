/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffWrapper.h
 *
 * @brief Declaration file for CDirColsDlg
 *
 * @date  Created: 2003-08-22
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _DIFFWRAPPER_H
#define _DIFFWRAPPER_H

/**
 * @brief Operations in diffranges.
 * DIFFRANGE structs op-member can have these values
 */
enum
{
	OP_NONE = 0,
	OP_LEFTONLY,
	OP_DIFF,
	OP_RIGHTONLY
};

/**
 * @brief One difference defined by linenumbers
 * @note xxxx1 values are calculated in CMergeDoc::PrimeTextBuffers()
 */
typedef struct tagDIFFRANGE
{
	UINT begin0,end0,begin1,end1;
	UINT dbegin0,dend0,dbegin1,dend1;
	int blank0,blank1;
	BYTE op;
} DIFFRANGE;

/**
 * @brief Diffutils options
 * @note DO NOT add diff-unrelated data here!
 */
typedef struct tagDIFFSETTINGS
{
	enum output_style outputStyle;
	int context;
	int alwaysText;
	int horizLines;
	int ignoreSpaceChange;
	int ignoreAllSpace;
	int ignoreBlankLines;
	int ignoreCase;
	int heuristic;
	int recursive;
} DIFFSETTINGS;

/**
 * @brief Diffutils returns this statusdata about files compared
 */
typedef struct tagDIFFSTATUS
{
	BOOL bLeftMissingNL;
	BOOL bRightMissingNL;
	BOOL bBinaries;
	BOOL bPatchFileFailed;
} DIFFSTATUS;

/**
 * @brief Wrapper class for GNU/diffutils
 */
class CDiffWrapper
{
public:
	CDiffWrapper();
	void SetCompareFiles(CString file1, CString file2);
	void SetPatchFile(CString file);
	void SetDiffList(CArray<DIFFRANGE,DIFFRANGE> *diffs);
	void GetOptions(DIFFSETTINGS *options);
	void SetOptions(DIFFSETTINGS *options);
	BOOL GetUseDiffList() const;
	BOOL SetUseDiffList(BOOL bUseDiffList);
	BOOL GetAppendFiles() const;
	BOOL SetAppendFiles(BOOL bAppendFiles);
	BOOL GetCreatePatchFile() const;
	BOOL SetCreatePatchFile(BOOL bCreatePatchFile);
	BOOL RunFileDiff();
	void GetDiffStatus(DIFFSTATUS *status);
	void AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op);
	void FixLastDiffRange(int leftBufferLines, int rightBufferLines, BOOL left);

protected:
	void SwapToInternalOptions();
	void SwapToGlobalOptions();
	CString FormatSwitchString();

private:
	DIFFSETTINGS m_options;
	DIFFSETTINGS m_globalOptions;	// Temp for storing globals
	DIFFSTATUS m_status;
	CString m_sFile1;
	CString m_sFile2;
	CString m_sPatchFile;
	BOOL m_bUseDiffList;
	BOOL m_bCreatePatchFile;
	BOOL m_bAddCmdLine;
	BOOL m_bAppendFiles;
	int m_nDiffs;
	CArray<DIFFRANGE,DIFFRANGE> *m_diffs;
};

#endif // _DIFFWRAPPER_H