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
		COMPAREFLAGS=0x7000, SAME=0x1000, DIFF=0x2000, SKIPPED=0x3000, CMPERR=0x4000, NOCMP=0x0000
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
	bool isResultDiff() const { return (!isResultSame() && !isResultSkipped() && !isResultError() &&
			!isSideLeft() && !isSideRight()); }
	bool isResultSkipped() const { return ((diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SKIPPED); }
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

// Interface for testing files & directories for exclusion, as diff traverses file tree
class IDiffFilter
{
public:
	virtual BOOL includeFile(LPCTSTR szFileName)=0;
	virtual BOOL includeDir(LPCTSTR szDirName)=0;
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

	void SetRegExp(LPCTSTR pszExp);

	// add & remove differences
	void AddDiff(const CString &pszFilename, const CString &szSubdir, const CString &pszLeftDir, const CString &pszRightDir
		, __int64 lmtime, __int64 rmtime, __int64 lctime, __int64 rctime
		, __int64 lsize, __int64 rsize, int diffcode
		, int lattrs=0, int rattrs=0
		, int ndiffs=-1, int ntrivialdiffs=-1);
	void AddDiff(DIFFITEM & di);
	void RemoveDiff(POSITION diffpos);
	void RemoveAll();
	void UpdateFieldsNeededForNewItems(DIFFITEM & di, DiffFileInfo & dfi);
	void UpdateVersion(DIFFITEM & di, DiffFileInfo & dfi);

	// to iterate over all differences on list
	POSITION GetFirstDiffPosition();
	DIFFITEM GetNextDiffPosition(POSITION & diffpos);
	DIFFITEM & GetDiffAt(POSITION diffpos);
	const DIFFITEM & GetDiffAt(POSITION diffpos) const;
//	int GetDiffStatus(POSITION diffpos);
	int GetDiffCount();

	// change an existing difference
	void SetDiffStatusCode(POSITION diffpos, UINT diffcode, UINT mask);
	void UpdateInfoFromDisk(DIFFITEM & di);
	void UpdateInfoFromDiskHalf(DIFFITEM & di, DiffFileInfo & dfi);
	void UpdateStatusFromDisk(POSITION diffpos);

	// retrieve or manufacture plugin info for specified file comparison
	void FetchPluginInfos(const CString& filteredFilenames, PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);

	BOOL m_bRecurse;
	CString m_strLeft;
	CString m_strRight;
	CString m_strNormalizedLeft;
	CString m_strNormalizedRight;
	CString m_strRegExp;
	IDiffFilter * m_piFilterUI;
	IDiffFilter * m_piFilterGlobal;
	IPluginInfos * m_piPluginInfos;
	UINT m_msgUpdateStatus;
	HWND m_hDirFrame;

	struct dirdata ddLeft, ddRight;
	char *pNamesLeft;
	char *pNamesRight;

private:
	CList<DIFFITEM,DIFFITEM> m_dirlist, *m_pList; // master list of differences
};

#endif // !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
