/** 
 * @file  CompareStats.cpp
 *
 * @brief Implementation of CompareStats class.
 */

#include "CompareStats.h"
#include <cassert>
#include <cstring>
#include <Poco/ScopedLock.h>
#include "DiffItem.h"

using Poco::FastMutex;

/** 
 * @brief Constructor, initializes critical section.
 */
CompareStats::CompareStats(int nDirs)
: m_nTotalItems(0)
, m_nComparedItems(0)
, m_state(STATE_IDLE)
, m_bCompareDone(false)
, m_nDirs(nDirs)
, m_counts()
{
}

/** 
 * @brief Destructor, deletes critical section.
 */
CompareStats::~CompareStats()
{
}

/** 
 * @brief Increase found items (dirs and files) count.
 * @param [in] count Amount of items to add.
 */
void CompareStats::IncreaseTotalItems(int count /*= 1*/)
{
	FastMutex::ScopedLock lock(m_csProtect);
	m_nTotalItems += count;
}

/** 
 * @brief Add compared item.
 * @param [in] code Resultcode to add.
 */
void CompareStats::AddItem(int code)
{
	FastMutex::ScopedLock lock(m_csProtect);
	RESULT res = GetResultFromCode(code);
	int index = static_cast<int>(res);
	m_counts[index] += 1;
	++m_nComparedItems;
	assert(m_nComparedItems <= m_nTotalItems);
}

/** 
 * @brief Return count by resultcode.
 * @param [in] result Resultcode to return.
 * @return Count of items for given resultcode.
 */
int CompareStats::GetCount(CompareStats::RESULT result) const
{
	return m_counts[result];
}

/** 
 * @brief Return total count of items (so far) found.
 */
int CompareStats::GetTotalItems() const
{
	return m_nTotalItems;
}

/**
* @brief Return item taking most time among current items.
*/
const DIFFITEM *CompareStats::GetCurDiffItem()
{
	int nHitCountMax = 0;
	const DIFFITEM *cdi = m_rgThreadState.front().m_pDiffItem;
	std::vector<ThreadState>::iterator it = m_rgThreadState.begin();
	while (it != m_rgThreadState.end())
	{
		const DIFFITEM *di = it->m_pDiffItem;
		if (di != NULL && (di->diffcode.diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::NOCMP)
		{
			int nHitCount = it->m_nHitCount++;
			if (nHitCountMax < nHitCount)
			{
				nHitCountMax = nHitCount;
				cdi = di;
			}
		}
		++it;
	}
	return cdi;
}

/** 
 * @brief Reset comparestats.
 * Use this function to reset stats before new compare.
 */
void CompareStats::Reset()
{
	std::fill(std::begin(m_counts), std::end(m_counts), 0);
	SetCompareState(STATE_IDLE);
	m_nTotalItems = 0;
	m_nComparedItems = 0;
	m_bCompareDone = false;
}

/** 
 * @brief Change compare state.
 * @param [in] state New compare state.
 */
void CompareStats::SetCompareState(CompareStats::CMP_STATE state)
{
	// New compare starting so reset ready status
	if (state == STATE_START)
		m_bCompareDone = false;
	// Compare ready
	if (state == STATE_IDLE && m_state == STATE_COMPARE)
		m_bCompareDone = true;

	m_state = state;
}

/** 
 * @brief Return current comparestate.
 */
CompareStats::CMP_STATE CompareStats::GetCompareState() const
{
	return m_state;
}

/** 
 * @brief Convert diffcode to compare-result.
 * @param [in] diffcode DIFFITEM.diffcode to convert.
 * @return Compare result.
 */
CompareStats::RESULT CompareStats::GetResultFromCode(unsigned diffcode) const
{
	DIFFCODE di(diffcode);
	
	// Test first for skipped so we pick all skipped items as such 
	if (di.isResultFiltered())
	{
		// skipped
		return di.isDirectory() ? RESULT_DIRSKIP : RESULT_SKIP;
	}
	else if (di.isSideFirstOnly())
	{
		// left-only
		return di.isDirectory() ? RESULT_LDIRUNIQUE : RESULT_LUNIQUE;
	}
	else if (di.isSideSecondOnly())
	{
		// right-only
		if (di.isDirectory())
			return (m_nDirs < 3) ? RESULT_RDIRUNIQUE : RESULT_MDIRUNIQUE;
		else
			return (m_nDirs < 3) ? RESULT_RUNIQUE : RESULT_MUNIQUE;
	}
	else if (di.isSideThirdOnly())
	{
		// right-only
		return di.isDirectory() ? RESULT_RDIRUNIQUE : RESULT_RUNIQUE;
	}
	else if (m_nDirs > 2 && !di.exists(0) && di.exists(1) && di.exists(2))
		return di.isDirectory() ? RESULT_LDIRMISSING : RESULT_LMISSING;
	else if (m_nDirs > 2 && di.exists(0) && !di.exists(1) && di.exists(2))
		return di.isDirectory() ? RESULT_MDIRMISSING : RESULT_MMISSING;
	else if (m_nDirs > 2 && di.exists(0) && di.exists(1) && !di.exists(2))
		return di.isDirectory() ? RESULT_RDIRMISSING : RESULT_RMISSING;
	else if (di.isResultError())
	{
		// could be directory error ?
		return RESULT_ERROR;
	}
	// Now we know it was on both sides & compared!
	else if (di.isResultSame())
	{
		// same
		return di.isBin() ? RESULT_BINSAME : RESULT_SAME;
	}
	else
	{
		// presumably it is diff
		if (di.isDirectory())
		{
			return RESULT_DIR;
		}
		else
		{
			return di.isBin() ? RESULT_BINDIFF : RESULT_DIFF;
		}
	}
}

void CompareStats::Swap(int idx1, int idx2)
{
	idx2 = m_nDirs < 3 ? idx2 + 1 : idx2;
	std::swap(m_counts[RESULT_LUNIQUE + idx1], m_counts[RESULT_LUNIQUE + idx2]);
	std::swap(m_counts[RESULT_LMISSING + idx1], m_counts[RESULT_LMISSING + idx2]);
	std::swap(m_counts[RESULT_LDIRUNIQUE + idx1], m_counts[RESULT_LDIRUNIQUE + idx2]);
	std::swap(m_counts[RESULT_LDIRMISSING + idx1], m_counts[RESULT_LDIRMISSING + idx2]);
}
