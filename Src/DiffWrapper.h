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
 * @brief Declaration file for CDiffWrapper
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
 * @brief One difference defined by linenumbers.
 *
 * This struct defines one set of different lines "diff".
 * @p begin0, @p end0, @p begin1 & @p end1 are linenumbers
 * in original files. Other struct members point to linenumbers
 * calculated by WinMerge after adding empty lines to make diffs
 * be in line in screen.
 *
 * @note @p blank0 & @p blank1 are -1 if there are no blank lines
 */
typedef struct tagDIFFRANGE
{
	UINT begin0;	/**< First diff line in original file1 */
	UINT end0;		/**< Last diff line in original file1 */
	UINT begin1;	/**< First diff line in original file2 */
	UINT end1;		/**< Last diff line in original file2 */
	UINT dbegin0;	/**< First deleted line in file1 */
	UINT dend0;		/**< Last deleted line in file1 */
	UINT dbegin1;	/**< First deleted line in file2 */
	UINT dend1;		/**< Last deleted line in file2 */
	int blank0;		/**< Number of blank lines in file1 */
	int blank1;		/**< Number of blank lines in file2 */
	BYTE op;		/**< Operation done with this diff */
} DIFFRANGE;

/**
 * @brief Diffutils options users of this class must use
 */
typedef struct tagDIFFOPTIONS
{
	int nIgnoreWhitespace;
	BOOL bIgnoreCase;
	BOOL bIgnoreBlankLines;
	BOOL bEolSensitive;
} DIFFOPTIONS;

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
 * @brief Internally used diffutils options
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
	int ignoreEOLDiff;
	int ignoreSomeChanges;
	int lengthVaries;
	int heuristic;
	int recursive;
} DIFFSETTINGS;

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
	void GetOptions(DIFFOPTIONS *options);
	void SetOptions(DIFFOPTIONS *options);
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
	void StartDirectoryDiff();
	void EndDirectoryDiff();
	static void ReadDiffOptions(DIFFOPTIONS *options);
	static void WriteDiffOptions(DIFFOPTIONS *options);

protected:
	void InternalGetOptions(DIFFOPTIONS *options);
	void InternalSetOptions(DIFFOPTIONS *options);
	void SwapToInternalSettings();
	void SwapToGlobalSettings();
	CString FormatSwitchString();

private:
	DIFFSETTINGS m_settings;
	DIFFSETTINGS m_globalSettings;	// Temp for storing globals
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