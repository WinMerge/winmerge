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

class CDiffContext;
class PrediffingInfo;

/**
 * @brief Different compare methods
 */
enum
{
	CMP_CONTENT = 0, /**< Normal by content compare */
	CMP_DATE, /**< Compare by modified date */
};

/**
 * @brief Operations in diffranges.
 * DIFFRANGE structs op-member can have these values
 */
enum
{
	OP_NONE = 0,
	OP_LEFTONLY,
	OP_DIFF,
	OP_RIGHTONLY,
	OP_TRIVIAL
};

/**
 * @brief Whitespace options
 *
 * DIFFOPTIONS structs nIgnoreWhitespace can have these values
 */
enum
{
	WHITESPACE_COMPARE_ALL = 0,
	WHITESPACE_IGNORE_CHANGE,
	WHITESPACE_IGNORE_ALL
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
struct DIFFRANGE
{
	UINT begin0;	/**< First diff line in original file1 */
	UINT end0;		/**< Last diff line in original file1 */
	UINT begin1;	/**< First diff line in original file2 */
	UINT end1;		/**< Last diff line in original file2 */
	UINT dbegin0;	/**< Synchronised (ghost lines added) first diff line in file1 */
	UINT dend0;		/**< Synchronised (ghost lines added) last diff line in file1 */
	UINT dbegin1;	/**< Synchronised (ghost lines added) first diff line in file2 */
	UINT dend1;		/**< Synchronised (ghost lines added) last diff line in file2 */
	int blank0;		/**< Number of blank lines in file1 */
	int blank1;		/**< Number of blank lines in file2 */
	BYTE op;		/**< Operation done with this diff */
};

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
	BOOL bBinariesIdentical; /**< Binary files are identical. */
	BOOL bPatchFileFailed; /**< Creating patch file failed */
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
	void SetCompareFiles(CString file1, CString file2);
	void SetPatchFile(CString file);
	void SetDiffList(CArray<DIFFRANGE,DIFFRANGE> *diffs);
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
	BOOL RunFileDiff();
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

private:
	DIFFSETTINGS m_settings;
	DIFFSETTINGS m_globalSettings;	// Temp for storing globals
	DIFFSTATUS m_status;
	CString m_sFile1;
	CString m_sFile2;
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
	CArray<DIFFRANGE,DIFFRANGE> *m_diffs;
	CMap<int, int, int, int> m_moved0;
	CMap<int, int, int, int> m_moved1;
};

// forward declarations needed by DiffFileData
struct file_data;
class PrediffingInfo;

/**
 * @brief C++ container for the structure (file_data) used by diffutils' diff_2_files(...)
 */
struct DiffFileData
{
// class interface

	static void SetDefaultCodepage(int defcp); // set codepage to assume for all unknown files

// instance interface

	DiffFileData();
	~DiffFileData();

	bool OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2);
	void Reset();
	void Close() { Reset(); }

	int just_compare_files(int depth);
	void GuessEncoding(int side, CDiffContext * pCtxt);
	int prepAndCompareTwoFiles(CDiffContext * pCtxt, const CString & filepath1, const CString & filepath2);

	file_data * m_inf;
	bool m_used; // whether m_inf has real data
	struct Filepath : CString
	{
		int unicoding;
		int codepage;
		Filepath():unicoding(0),codepage(0)
		{
		}
		bool Transform(const CString & filepath, CString & filepathTransformed,
			const CString & filteredFilenames, PrediffingInfo * infoPrediffer, int fd);
		void GuessEncoding(const char **data, int count);
	} m_sFilepath[2];
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
