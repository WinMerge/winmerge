/**
 *  @file DiffContext.h
 *
 *  @brief Declarations of CDiffContext and diff structures
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
#define AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_
#pragma once

#ifndef _DIFF_FILE_INFO_H_INCLUDED
#include "DiffFileInfo.h"
#endif

class PackingInfo;
class PrediffingInfo;
class IDiffFilter;

struct dirdata
{
  char const **names;	/* Sorted names of files in dir, 0-terminated.  */
  char *data;	/* Allocated storage for file names.  */
};

// values for DIFFITEM.code
struct DIFFCODE
{
	enum
	{
		// We use extra bits so that no valid values are 0
		// and each set of flags is in a different hex digit
		// to make debugging easier
		// These can always be packed down in the future
		TEXTFLAG=0x3, TEXT=0x1, BIN=0x2,
		DIRFLAG=0x30, FILE=0x10, DIR=0x20,
		SIDEFLAG=0x300, LEFT=0x100, RIGHT=0x200, BOTH=0x300,
		COMPAREFLAGS=0x7000, NOCMP=0x0000, SAME=0x1000, DIFF=0x2000, CMPERR=0x4000,
		FILTERFLAGS=0x30000, INCLUDED=0x10000, SKIPPED=0x20000,
	};

	int diffcode;

	DIFFCODE(int diffcode) : diffcode(diffcode) { }
	// file/directory
	bool isDirectory() const { return ((diffcode & DIFFCODE::DIRFLAG) == DIFFCODE::DIR); }
	// left/right
	bool isSideLeft() const { return ((diffcode & DIFFCODE::SIDEFLAG) == DIFFCODE::LEFT); }
	bool isSideRight() const { return ((diffcode & DIFFCODE::SIDEFLAG) == DIFFCODE::RIGHT); }
	// result filters
	bool isResultError() const { return ((diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::CMPERR); }
	bool isResultSame() const { return ((diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME); }
	bool isResultDiff() const { return (!isResultSame() && !isResultFiltered() && !isResultError() &&
			!isSideLeft() && !isSideRight()); }
	bool isResultFiltered() const { return ((diffcode & DIFFCODE::FILTERFLAGS) == DIFFCODE::SKIPPED); }
	// type
	bool isBin() const { return ((diffcode & DIFFCODE::TEXTFLAG) == DIFFCODE::BIN); }
};

class CDiffContext;

/**
 * @brief information about one diff (including files on both sides)
 *
 * Bitmask can be seen as a 4 dimensional space; that is, there are four
 * different attributes, and each entry picks one of each attribute
 * independently.
 *
 * One dimension is how the compare went: same or different or
 * skipped or error.
 *
 * One dimension is file mode: text or binary (text is only if
 * both sides were text)
 *
 * One dimension is existence: both sides, left only, or right only
 *
 * One dimension is type: directory, or file
 */

struct DIFFITEM : DIFFCODE
{
	DiffFileInfo left;
	DiffFileInfo right;
	CString sfilename;
	CString sSubdir; //*< Common subdirectory from root of comparison */
	int	nsdiffs;
	int ndiffs;

	DIFFITEM() : DIFFCODE(0), ndiffs(-1), nsdiffs(-1) { }

	CString getLeftFilepath(const CDiffContext *) const;
	CString getRightFilepath(const CDiffContext *) const;
};

// Interface for reporting current file, as diff traverses file tree
class IDiffStatus
{
public:
	virtual void rptFile(BYTE code)=0;
};

/** Interface to a provider of plugin info */
class IPluginInfos
{
public:
	virtual void FetchPluginInfos(const CString& filteredFilenames, 
                                      PackingInfo ** infoUnpacker, 
                                      PrediffingInfo ** infoPrediffer) = 0;
};


class CDiffContext  
{
public:
	CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight);
	CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight, CDiffContext& src);
	virtual ~CDiffContext();

	// add & remove differences
	void AddDiff(DIFFITEM & di);
	void RemoveDiff(POSITION diffpos);
	void RemoveAll();
	void UpdateVersion(DIFFITEM & di, DiffFileInfo & dfi) const;

	//@{
	/**
	 * @name Path accessor functions.
	 *
	 * These functions return left/right path associated to DiffContext.
	 * There is no setter fuctions and path can be set only via constructor.
	 * Normalized paths are preferred to use - short paths are expanded
	 * and trailing slashes removed (except from root path).
	 */
	const CString & GetLeftPath() const { return m_strLeft; }
	const CString & GetRightPath() const { return m_strRight; }
	const CString & GetNormalizedLeft() const { return m_strNormalizedLeft; }
	const CString & GetNormalizedRight() const { return m_strNormalizedRight; }
	//@}

	// to iterate over all differences on list
	POSITION GetFirstDiffPosition();
	DIFFITEM GetNextDiffPosition(POSITION & diffpos);
	const DIFFITEM & GetDiffAt(POSITION diffpos) const;
//	int GetDiffStatus(POSITION diffpos);
	int GetDiffCount();

	// change an existing difference
	void SetDiffStatusCode(POSITION diffpos, UINT diffcode, UINT mask);
	void SetDiffCounts(POSITION diffpos, UINT diffs, UINT ignored);
	void UpdateInfoFromDiskHalf(DIFFITEM & di, DiffFileInfo & dfi);
	void UpdateStatusFromDisk(POSITION diffpos, BOOL bLeft, BOOL bRight);

	// retrieve or manufacture plugin info for specified file comparison
	void FetchPluginInfos(const CString& filteredFilenames, PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);

	BOOL m_bRecurse;
	IDiffFilter * m_piFilterGlobal;
	IPluginInfos * m_piPluginInfos;
	UINT m_msgUpdateStatus;
	HWND m_hDirFrame;
	BOOL m_bGuessEncoding;

	struct dirdata ddLeft, ddRight;
	char *pNamesLeft;
	char *pNamesRight;

private:
	CList<DIFFITEM,DIFFITEM> m_dirlist, *m_pList; // master list of differences

	CString m_strLeft;
	CString m_strRight;
	CString m_strNormalizedLeft;
	CString m_strNormalizedRight;
};

#endif // !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
