/**
 *  @file CompareStats.h
 *
 *  @brief Declaration of class CompareStats
 */ 
#pragma once

#include <atomic>
#include <vector>
#include <array>

class DIFFITEM;

/**
 * @brief Class holding directory compare stats.
 *
 * This class is used for sharing compare stats between dir compare
 * classes. CDiffContext updates statuses. GUI (compare statepane) tracks
 * state changes and updates UI. If compare is fast (compared few small files)
 * GUI might not be able to detect state change from IDLE back to IDLE. That's
 * why there is IsCompareDone() which tells if new compare is ready.
 */
class CompareStats
{
public:

	/**
	* @brief Different states for compare procedure.
	* These states form state-machine for directory compare. States go like:
	* STATE_IDLE --> STATE_START --> STATE_COMPARE --> STATE_IDLE.
	* @note GUI doesn't change state, but only backend code. GUI must track
	* state changes to update itself.
	*/
	enum CMP_STATE
	{
		STATE_IDLE, /**< No compare running */
		STATE_START, /**< Start folder compare */
		STATE_COMPARE, /**< Comparing collected items */
	};

	/**
	* @brief Resultcodes we store.
	*/
	enum RESULT
	{
		RESULT_LUNIQUE = 0,
		RESULT_MUNIQUE,
		RESULT_RUNIQUE,
		RESULT_LMISSING,
		RESULT_MMISSING,
		RESULT_RMISSING,
		RESULT_DIFF,
		RESULT_SAME,
		RESULT_BINSAME,
		RESULT_BINDIFF,
		RESULT_LDIRUNIQUE,
		RESULT_MDIRUNIQUE,
		RESULT_RDIRUNIQUE,
		RESULT_LDIRMISSING,
		RESULT_MDIRMISSING,
		RESULT_RDIRMISSING,
		RESULT_SKIP,
		RESULT_DIRSKIP,
		RESULT_DIRSAME,
		RESULT_DIRDIFF,
		RESULT_ERROR,
		RESULT_COUNT  //THIS MUST BE THE LAST ITEM
	};

	explicit CompareStats(int nDirs);
	~CompareStats();
	void SetCompareThreadCount(int nCompareThreads)
	{
		m_rgThreadState.resize(nCompareThreads);
	}
	void BeginCompare(const DIFFITEM *di, int iCompareThread)
	{
		ThreadState &rThreadState = m_rgThreadState[iCompareThread];
		rThreadState.m_nHitCount = 0;
		rThreadState.m_pDiffItem = di;
	}
	void AddItem(int code);
	void IncreaseTotalItems(int count = 1);
	int GetCount(CompareStats::RESULT result) const;
	int GetTotalItems() const;
	int GetComparedItems() const { return m_nComparedItems; }
	const DIFFITEM *GetCurDiffItem();
	void Reset();
	void SetCompareState(CompareStats::CMP_STATE state);
	CompareStats::CMP_STATE GetCompareState() const;
	bool IsCompareDone() const { return m_bCompareDone; }
	CompareStats::RESULT GetResultFromCode(unsigned diffcode) const;
	void Swap(int idx1, int idx2);
	int GetCompareDirs() const { return m_nDirs; }

private:
	std::array<std::atomic_int, RESULT_COUNT> m_counts; /**< Table storing result counts */
	std::atomic_int m_nTotalItems; /**< Total items found to compare */
	std::atomic_int m_nComparedItems; /**< Compared items so far */
	CMP_STATE m_state; /**< State for compare (idle, collect, compare,..) */
	bool m_bCompareDone; /**< Have we finished last compare? */
	int m_nDirs; /**< number of directories to compare */
	struct ThreadState
	{
		ThreadState() : m_nHitCount(0), m_pDiffItem(nullptr) {}
		ThreadState(const ThreadState& other) : m_nHitCount(other.m_nHitCount.load()), m_pDiffItem(other.m_pDiffItem) {}
		std::atomic_int m_nHitCount;
		const DIFFITEM *m_pDiffItem;
	};
	std::vector<ThreadState> m_rgThreadState;

};

/** 
 * @brief Increase found items (dirs and files) count.
 * @param [in] count Amount of items to add.
 */
inline void CompareStats::IncreaseTotalItems(int count)
{
	m_nTotalItems += count;
}

/** 
 * @brief Return count by resultcode.
 * @param [in] result Resultcode to return.
 * @return Count of items for given resultcode.
 */
inline int CompareStats::GetCount(CompareStats::RESULT result) const
{
	return m_counts[result];
}

/**
 * @brief Return total count of items (so far) found.
 */
inline int CompareStats::GetTotalItems() const
{
	return m_nTotalItems;
}

/**
 * @brief Return current comparestate.
 */
inline CompareStats::CMP_STATE CompareStats::GetCompareState() const
{
	return m_state;
}
