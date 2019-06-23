/**
 *  @file DiffContext.h
 *
 *  @brief Declarations of CDiffContext and diff structures
 */
#pragma once

#define POCO_NO_UNWINDOWS 1
#include <Poco/Mutex.h>
#include <memory>
#include "PathContext.h"
#include "DiffItemList.h"
#include "FilterList.h"

class PackingInfo;
class PrediffingInfo;
class IDiffFilter;
class CompareStats;
class IAbortable;
class CDiffWrapper;
class CompareOptions;
struct DIFFOPTIONS;
class FilterCommentsManager;

/** Interface to a provider of plugin info */
class IPluginInfos
{
public:
	virtual void FetchPluginInfos(const String& filteredFilenames, 
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

	CDiffContext(const PathContext & paths, int compareMethod);
	~CDiffContext();

	void UpdateVersion(DIFFITEM &di, int nIndex) const;

	/**
	 * Get the main compare method used in this compare.
	 * @return Compare method used.
	 */
	int GetCompareMethod(void) const { return m_nCompMethod; }

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
	String GetLeftPath() const { return m_paths.GetLeft(false); }
	String GetMiddlePath() const { return m_paths.GetMiddle(false); }
	/**
	 * Get right-side compare path.
	 * @return full path in right-side.
	 */
	String GetRightPath() const { return m_paths.GetRight(false); }
	String GetPath(int nIndex) const { return m_paths.GetPath(nIndex, false); }
	/**
	 * Get left-side compare path in normalized form.
	 * @return full path in left-side.
	 */
	String GetNormalizedLeft() const { return m_paths.GetLeft(); }
	String GetNormalizedMiddle() const { return m_paths.GetMiddle(); }
	/**
	 * Get right-side compare path in normalized form.
	 * @return full path in left-side.
	 */
	String GetNormalizedRight() const { return m_paths.GetRight(); }
	String GetNormalizedPath(int nIndex) const { return m_paths.GetPath(nIndex, true); }
	PathContext GetNormalizedPaths() const
	{
		PathContext paths;
		for (int nIndex = 0; nIndex < m_paths.GetSize(); nIndex++)
			paths.SetPath(nIndex, m_paths.GetPath(nIndex, true));
		return paths;
	}
	//@}

	// change an existing difference
	bool UpdateInfoFromDiskHalf(DIFFITEM &di, int nIndex);
	void UpdateStatusFromDisk(DIFFITEM *diffpos, int nIndex);

	bool CreateCompareOptions(int compareMethod, const DIFFOPTIONS & options);
	CompareOptions * GetCompareOptions(int compareMethod);

	// retrieve or manufacture plugin info for specified file comparison
	void FetchPluginInfos(const String& filteredFilenames,
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

	int GetCompareDirs() const { return m_paths.GetSize(); }

	void Swap(int idx1, int idx2)
	{
		String tmp;
		tmp = m_paths.GetPath(idx1);
		m_paths.SetPath(idx1, m_paths.GetPath(idx2));
		m_paths.SetPath(idx2, tmp);
		DiffItemList::Swap(idx1, idx2);
	}

	IDiffFilter * m_piFilterGlobal; /**< Interface for file filtering. */
	IPluginInfos * m_piPluginInfos;
	int m_iGuessEncodingType;

	bool m_bIgnoreSmallTimeDiff; /**< Ignore small timedifferences when comparing by date */
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

	int m_nBinaryCompareLimit;

	/**
	 * Walk into unique folders and add contents.
	 * This enables/disables walking into unique folders. If we don't walk into
	 * unique folders, they are shown as such in folder compare results. If we
	 * walk into unique folders, we'll show all files in the unique folder and
	 * in possible subfolders.
	 *
	 * This value is true by default.
	 */
	bool m_bWalkUniques;
	bool m_bIgnoreReparsePoints;
	bool m_bIgnoreCodepage;

	bool m_bRecursive; /**< Do we include subfolders to compare? */
	bool m_bPluginsEnabled; /**< Are plugins enabled? */
	std::unique_ptr<FilterList> m_pFilterList; /**< Filter list for line filters */
	FilterCommentsManager *m_pFilterCommentsManager;

private:
	/**
	 * The main compare method used.
	 * This is the main compare method set when compare is started. There
	 * can be temporary switches to other method (e.g. for large file) but
	 * this main method must be set back for next file.
	 */
	int m_nCompMethod;

	std::unique_ptr<DIFFOPTIONS> m_pOptions; /**< Generalized compare options. */
	std::unique_ptr<CompareOptions> m_pContentCompareOptions; /**< Per compare method compare options. */
	std::unique_ptr<CompareOptions> m_pQuickCompareOptions;   /**< Per compare method compare options. */
	PathContext m_paths; /**< (root) paths for this context */
	IAbortable *m_piAbortable; /**< Interface for aborting the compare. */
	Poco::FastMutex m_mutex;
};
