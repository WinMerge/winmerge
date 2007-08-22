/**
 *  @file DiffContext.h
 *
 *  @brief Declarations of CDiffContext and diff structures
 */
// ID line follows -- this is updated by SVN
// $Id$

#if !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
#define AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_
#pragma once

#ifndef _PATH_CONTEXT_H_
#include "PathContext.h"
#endif

#ifndef _DIFF_FILE_INFO_H_INCLUDED
#include "DiffFileInfo.h"
#endif

#ifndef _DIFF_ITEM_LIST_H_
#include "DiffItemList.h"
#endif

class PackingInfo;
class PrediffingInfo;
class IDiffFilter;
struct DIFFITEM;
class CompareStats;
class IAbortable;
class FilterList;
class CompareOptions;
struct DIFFOPTIONS;

// Interface for reporting current file, as diff traverses file tree
/*class IDiffStatus
{
public:
	virtual void rptFile(BYTE code)=0;
};
*/

/** Interface to a provider of plugin info */
class IPluginInfos
{
public:
	virtual void FetchPluginInfos(const CString& filteredFilenames, 
                                      PackingInfo ** infoUnpacker, 
                                      PrediffingInfo ** infoPrediffer) = 0;
};

/**
 * @brief Directory compare context.
 *
 * @note If you add new member variables, remember to copy values in
 * CDiffContext::CDiffContext(..,CDiffContext) constructor!
 */
class CDiffContext : public DiffItemList
{
public:
	enum
	{
		DIFFS_UNKNOWN = -1,
		DIFFS_UNKNOWN_QUICKCOMPARE = -9,
	};

	CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight);
	CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight, CDiffContext& src);
	~CDiffContext();

	// add & remove differences
	virtual void AddDiff(const DIFFITEM & di);
	void UpdateVersion(DIFFITEM & di, BOOL bLeft) const;

	//@{
	/**
	 * @name Path accessor functions.
	 *
	 * These functions return left/right path associated to DiffContext.
	 * There is no setter fuctions and path can be set only via constructor.
	 * Normalized paths are preferred to use - short paths are expanded
	 * and trailing slashes removed (except from root path).
	 */
	CString GetLeftPath() const { return m_paths.GetLeft(FALSE); }
	CString GetRightPath() const { return m_paths.GetRight(FALSE); }
	CString GetNormalizedLeft() const { return m_paths.GetLeft(); }
	CString GetNormalizedRight() const { return m_paths.GetRight(); }
	//@}

	// change an existing difference
	BOOL UpdateInfoFromDiskHalf(DIFFITEM & di, BOOL bLeft);
	void UpdateStatusFromDisk(POSITION diffpos, BOOL bLeft, BOOL bRight);

	BOOL CreateCompareOptions(int compareMethod, const DIFFOPTIONS & options);
	CompareOptions * GetCompareOptions(int compareMethod);

	// retrieve or manufacture plugin info for specified file comparison
	void FetchPluginInfos(const CString& filteredFilenames,
		PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);

	bool ShouldAbort() const;
	void SetAbortable(IAbortable * piAbortable) { m_piAbortable = piAbortable; }
	const IAbortable * GetAbortable() const { return m_piAbortable; }

	IDiffFilter * m_piFilterGlobal;
	IPluginInfos * m_piPluginInfos;
	BOOL m_bGuessEncoding;
	int m_nCompMethod; /**< Compare method */
	BOOL m_bIgnoreSmallTimeDiff; /**< Ignore small timedifferences when comparing by date */
	CompareStats *m_pCompareStats; /**< Pointer to compare statistics */
	BOOL m_bStopAfterFirstDiff; /**< Optimize compare by stopping after first difference? */
	int m_nQuickCompareLimit; /**< Bigger files are always compared with quick compare */
	FilterList * m_pFilterList; /**< Filter list for line filters */
	CRITICAL_SECTION m_criticalSect; /**< Critical section protecting list access. */
	BOOL m_bCollectReady; /**< Tells collection phase is done. */

private:
	CList<DIFFITEM,DIFFITEM&> *m_pList; /**< Pointer to list, used to access list */
	DIFFOPTIONS *m_pOptions; /**< Generalized compare options. */
	CompareOptions *m_pCompareOptions; /**< Per compare method compare options. */
	PathContext m_paths; /**< (root) paths for this context */
	IAbortable *m_piAbortable;
};

#endif // !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
