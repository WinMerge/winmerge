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

#include "PathContext.h"
#include "DiffFileInfo.h"
#include "DiffItemList.h"

class PackingInfo;
class PrediffingInfo;
class IDiffFilter;
struct DIFFITEM;
class CompareStats;
class IAbortable;
class FilterList;
class CompareOptions;
struct DIFFOPTIONS;

/** Interface to a provider of plugin info */
class IPluginInfos
{
public:
	virtual void FetchPluginInfos(LPCTSTR filteredFilenames, 
                                      PackingInfo ** infoUnpacker, 
                                      PrediffingInfo ** infoPrediffer) = 0;
};

/**
 * The folder compare context.
 * This class holds data of the current folder compare. There are paths
 * to compare, filters used, compare options etc. And compare results list
 * is also contained in this class. Many compare classes and functions have
 * a pointer to instance of this class. 
 *
 * @note If you add new member variables, remember to copy values in
 * CDiffContext::CDiffContext(..,CDiffContext) constructor!
 */
class CDiffContext : public DiffItemList
{
public:
	/** @brief Special values for difference counts. */
	enum
	{
		DIFFS_UNKNOWN = -1, /**< Difference count unknown (generally). */
		DIFFS_UNKNOWN_QUICKCOMPARE = -9, /**< Unknown because of quick-compare method. */
	};

	CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight);
	~CDiffContext();

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
	/**
	 * Get left-side compare path.
	 * @return full path in left-side.
	 */
	String GetLeftPath() const { return m_paths.GetLeft(FALSE); }
	/**
	 * Get right-side compare path.
	 * @return full path in right-side.
	 */
	String GetRightPath() const { return m_paths.GetRight(FALSE); }
	/**
	 * Get left-side compare path in normalized form.
	 * @return full path in left-side.
	 */
	String GetNormalizedLeft() const { return m_paths.GetLeft(); }
	/**
	 * Get right-side compare path in normalized form.
	 * @return full path in left-side.
	 */
	String GetNormalizedRight() const { return m_paths.GetRight(); }
	//@}

	// change an existing difference
	BOOL UpdateInfoFromDiskHalf(DIFFITEM & di, BOOL bLeft);
	void UpdateStatusFromDisk(UINT_PTR diffpos, BOOL bLeft, BOOL bRight);

	BOOL CreateCompareOptions(int compareMethod, const DIFFOPTIONS & options);
	CompareOptions * GetCompareOptions(int compareMethod);

	// retrieve or manufacture plugin info for specified file comparison
	void FetchPluginInfos(LPCTSTR filteredFilenames,
		PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);

	//@{
	/**
	 * @name Compare aborting interface.
	 * These functions handle compare aborting using IAbortable interface.
	 */
	bool ShouldAbort() const;

	/**
	 * Set pointer to IAbortable interface.
	 * This function sets pointer to interface used to abort the compare when
	 * user wants to.
	 * @param [in] piAbortable Pointer to interface.
	 */
	void SetAbortable(IAbortable * piAbortable) { m_piAbortable = piAbortable; }

	/**
	 * Returns a pointer to current IAbortable interface.
	 * This function returns a pointer to interface used to abort the compare.
	 * @return Pointer to current IAbortable interface.
	 */
	const IAbortable * GetAbortable() const { return m_piAbortable; }
	//@}

	IDiffFilter * m_piFilterGlobal; /**< Interface for file filtering. */
	IPluginInfos * m_piPluginInfos;
	BOOL m_bGuessEncoding;

	/**
	 * The main compare method used.
	 * This is the main compare method set when compare is started. There
	 * can be temporary switches to other method (e.g. for large file) but
	 * this main method must be set back for next file.
	 */
	int m_nCompMethod;
	BOOL m_bIgnoreSmallTimeDiff; /**< Ignore small timedifferences when comparing by date */
	CompareStats *m_pCompareStats; /**< Pointer to compare statistics */

	/**
	 * Optimize compare by stopping after first difference.
	 * In some compare methods (currently quick compare) we can stop the
	 * compare right after finding the first difference. This speeds up the
	 * compare, but also causes compare statistics to be inaccurate.
	 */
	bool m_bStopAfterFirstDiff;

	/**
	 * Threshold size for switching to quick compare.
	 * When diffutils compare is selected, files bigger (in bytes) than this
	 * value are compared using Quick compare. This is because diffutils simply
	 * cannot compare large files. And large files are usually binary files.
	 */
	int m_nQuickCompareLimit;
	bool m_bPluginsEnabled; /**< Are plugins enabled? */
	FilterList * m_pFilterList; /**< Filter list for line filters */

private:
	DIFFOPTIONS *m_pOptions; /**< Generalized compare options. */
	CompareOptions *m_pCompareOptions; /**< Per compare method compare options. */
	PathContext m_paths; /**< (root) paths for this context */
	IAbortable *m_piAbortable; /**< Interface for aborting the compare. */
};

#endif // !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
