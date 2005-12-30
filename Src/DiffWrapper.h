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

#include "FileLocation.h"


class CDiffContext;
class PrediffingInfo;
struct DIFFRANGE;
class DiffList;
struct DiffFileData;
struct file_data;

/**
 * @brief Different compare methods
 */
enum
{
	CMP_CONTENT = 0, /**< Normal by content compare */
	CMP_QUICK_CONTENT, /**< Custom content compare */
	CMP_DATE, /**< Compare by modified date */
};

/**
 * @brief Patch styles
 *
 * Diffutils can output patch in these formats.
 */
enum
{
	/* Default output style.  */
	DIFF_OUTPUT_NORMAL,
	/* Output the differences with lines of context before and after (-c).  */
	DIFF_OUTPUT_CONTEXT,
	/* Output the differences in a unified context diff format (-u). */
	DIFF_OUTPUT_UNIFIED,
	/* Output the differences as commands suitable for `ed' (-e).  */
	DIFF_OUTPUT_ED,
	/* Output the diff as a forward ed script (-f).  */
	DIFF_OUTPUT_FORWARD_ED,
	/* Like -f, but output a count of changed lines in each "command" (-n). */
	DIFF_OUTPUT_RCS,
	/* Output merged #ifdef'd file (-D).  */
	DIFF_OUTPUT_IFDEF,
	/* Output sdiff style (-y).  */
	DIFF_OUTPUT_SDIFF
};

typedef enum {
	YESTEMPFILES // arguments are temp files
	, NOTEMPFILES // arguments not temp files
} ARETEMPFILES;
/**
 * @brief Diffutils options users of this class must use
 */
struct DIFFOPTIONS
{
	int nIgnoreWhitespace;
	BOOL bIgnoreCase;
	BOOL bIgnoreBlankLines;
	BOOL bEolSensitive;
};

/**
 * @brief Additional options for creating patch files
 */
struct PATCHOPTIONS
{
	enum output_style outputStyle;
	int nContext;
	BOOL bAddCommandline;
};

/**
 * @brief Diffutils returns this statusdata about files compared
 */
struct DIFFSTATUS
{
	BOOL bLeftMissingNL; /**< Left file is missing EOL before EOF */
	BOOL bRightMissingNL; /**< Right file is missing EOL before EOF */
	BOOL bBinaries; /**< Files are binaries */
	BOOL bIdentical; /**< diffutils said files are identical */
	BOOL bPatchFileFailed; /**< Creating patch file failed */

	DIFFSTATUS() { memset(this, 0, sizeof(*this)); } // start out with all flags clear
};

/**
 * @brief Internally used diffutils options
 */
struct DIFFSETTINGS
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
};

/**
 * @brief Wrapper class for GNU/diffutils
 */
class CDiffWrapper
{
public:
	CDiffWrapper();
	~CDiffWrapper();
	void SetPatchFile(CString file);
	void SetDiffList(DiffList *diffList);
	void GetOptions(DIFFOPTIONS *options);
	void SetOptions(DIFFOPTIONS *options);
	void SetTextForAutomaticPrediff(CString text);
	void SetPrediffer(PrediffingInfo * prediffer =NULL);
	void GetPrediffer(PrediffingInfo * prediffer);
	void GetPatchOptions(PATCHOPTIONS *options);
	void SetPatchOptions(PATCHOPTIONS *options);
	BOOL GetUseDiffList() const;
	BOOL SetUseDiffList(BOOL bUseDiffList);
	void SetDetectMovedBlocks(BOOL bDetectMovedBlocks) { m_bDetectMovedBlocks = bDetectMovedBlocks; }
	BOOL GetDetectMovedBlocks() { return m_bDetectMovedBlocks; }
	BOOL GetAppendFiles() const;
	BOOL SetAppendFiles(BOOL bAppendFiles);
	BOOL GetCreatePatchFile() const;
	BOOL SetCreatePatchFile(BOOL bCreatePatchFile);
	BOOL RunFileDiff(CString & filepath1, CString & filepath2, ARETEMPFILES areTempFiles);
	void GetDiffStatus(DIFFSTATUS *status);
	void AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op);
	void FixLastDiffRange(int leftBufferLines, int rightBufferLines, BOOL left);
	void StartDirectoryDiff();
	void EndDirectoryDiff();
	const CMap<int, int, int, int> * GetMoved0() { return &m_moved0; }
	const CMap<int, int, int, int> * GetMoved1() { return &m_moved1; }
	int RightLineInMovedBlock(int leftLine);
	int LeftLineInMovedBlock(int rightLine);
	void ClearMovedLists();

protected:
	void InternalGetOptions(DIFFOPTIONS *options);
	void InternalSetOptions(DIFFOPTIONS *options);
	void SwapToInternalSettings();
	void SwapToGlobalSettings();
	CString FormatSwitchString();
	BOOL Diff2Files(struct change ** diffs, DiffFileData *diffData,
		int * bin_status);
	void LoadWinMergeDiffsFromDiffUtilsScript(struct change * script, const file_data * inf);
	void WritePatchFile(struct change * script, const CString & filepath1, const CString & filepath2, file_data * inf);

private:
	DIFFSETTINGS m_settings;
	DIFFSETTINGS m_globalSettings;	// Temp for storing globals
	DIFFSTATUS m_status;
	CString m_sPatchFile;
	/// prediffer info are stored only for MergeDoc
	PrediffingInfo * m_infoPrediffer;
	/// prediffer info are stored only for MergeDoc
	CString m_sToFindPrediffer;
	BOOL m_bUseDiffList;
	BOOL m_bDetectMovedBlocks;
	BOOL m_bCreatePatchFile;
	BOOL m_bAddCmdLine;
	BOOL m_bAppendFiles;
	int m_nDiffs;
	DiffList *m_pDiffList;
	CMap<int, int, int, int> m_moved0;
	CMap<int, int, int, int> m_moved1;
};

// forward declarations needed by DiffFileData
struct file_data;
class PrediffingInfo;
struct DIFFITEM;
class IAbortable;

/**
 * @brief C++ container for the structure (file_data) used by diffutils' diff_2_files(...)
 */
struct DiffFileData
{
// class interface

	static void SetDefaultCodepage(int defcp); // set codepage to assume for all unknown files

// enums
	enum { DIFFS_UNKNOWN=-1, DIFFS_UNKNOWN_QUICKCOMPARE=-9 };

// instance interface

	DiffFileData();
	~DiffFileData();

	bool OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2);
	void Reset();
	void Close() { Reset(); }
	void SetDisplayFilepaths(LPCTSTR szTrueFilepath1, LPCTSTR szTrueFilepath2);

	int diffutils_compare_files(int depth);
	int byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable);
	void GuessEncoding(int side, CDiffContext * pCtxt);
	int prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di);
	BOOL Diff2Files(struct change ** diffs, int depth,
		int * bin_status, BOOL bMovedBlocks);
	bool Filepath_Transform(FileLocation & fpenc, const CString & filepath, CString & filepathTransformed,
		const CString & filteredFilenames, PrediffingInfo * infoPrediffer, int fd);
	void Filepath_GuessEncoding(FileLocation & fpenc, const char **data, int count);

// Data (public)
	file_data * m_inf;
	bool m_used; // whether m_inf has real data
	FileLocation m_FileLocation[2];

	CString m_sDisplayFilepath[2];
	int m_ndiffs;
	int m_ntrivialdiffs;
	struct UniFileBom // detect unicode file and quess encoding
	{
		UniFileBom(int); // initialize from file descriptor
		int size;
		int unicoding;
		unsigned char buffer[4];
	};

private:
	bool DoOpenFiles();
};

#endif // _DIFFWRAPPER_H
