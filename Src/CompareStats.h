/**
 *  @file CompareStats.h
 *
 *  @brief Declaration of class CompareStats
 */ 
#pragma once

#define POCO_NO_UNWINDOWS 1
#include <Poco/Mutex.h>
#include <Poco/AtomicCounter.h>
#include <vector>
#include <array>

struct DIFFITEM;

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
		RESULT_DIR,
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
	std::array<int, RESULT_COUNT> m_counts; /**< Table storing result counts */
	mutable Poco::FastMutex m_csProtect; /**< For synchronizing read/write of counts */
	long m_nTotalItems; /**< Total items found to compare */
	long m_nComparedItems; /**< Compared items so far */
	CMP_STATE m_state; /**< State for compare (idle, collect, compare,..) */
	bool m_bCompareDone; /**< Have we finished last compare? */
	int m_nDirs; /**< number of directories to compare */
	struct ThreadState
	{
		ThreadState() : m_nHitCount(0), m_pDiffItem(NULL) {}
		Poco::AtomicCounter m_nHitCount;
		const DIFFITEM *m_pDiffItem;
	};
	std::vector<ThreadState> m_rgThreadState;

};
