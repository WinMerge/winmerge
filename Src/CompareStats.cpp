/** 
 * @file  CompareStats.cpp
 *
 * @brief Implementation of CompareStats class.
 */

#include "pch.h"
#include "CompareStats.h"
#include <cassert>
#include "DiffItem.h"

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
, m_nIdleCompareThreadCount(0)
{
}

/** 
 * @brief Destructor, deletes critical section.
 */
CompareStats::~CompareStats() = default;

/** 
 * @brief Add compared item.
 * @param [in] code Resultcode to add.
 */
void CompareStats::AddItem(int code)
{
	if (code != -1)
	{
		RESULT res = GetResultFromCode(code);
		int index = static_cast<int>(res);
		m_counts[index]++;
	}
	++m_nComparedItems;
	assert(m_nComparedItems <= m_nTotalItems);
}

/**
* @brief Return item taking most time among current items.
*/
const DIFFITEM *CompareStats::GetCurDiffItem()
{
	if (m_rgThreadState.empty())
		return nullptr;
	int nHitCountMax = 0;
	const DIFFITEM *cdi = m_rgThreadState.front().m_pDiffItem;
	std::vector<ThreadState>::iterator it = m_rgThreadState.begin();
	while (it != m_rgThreadState.end())
	{
		const DIFFITEM *di = it->m_pDiffItem;
		if (di != nullptr && (di->diffcode.diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::NOCMP)
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
	m_rgThreadState.clear();
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
 * @brief Convert diffcode to compare-result.
 * @param [in] diffcode DIFFITEM.diffcode to convert.
 * @return Compare result.
 */
CompareStats::RESULT CompareStats::GetResultFromCode(unsigned diffcode) const
{
	DIFFCODE di(diffcode);
	
	bool is_dir = di.isDirectory();
	// Test first for skipped so we pick all skipped items as such 
	if (di.isResultFiltered())
	{
		// skipped
		return is_dir ? RESULT_DIRSKIP : RESULT_SKIP;
	}
	else if (di.isSideFirstOnly())
	{
		// left-only
		return is_dir ? RESULT_LDIRUNIQUE : RESULT_LUNIQUE;
	}
	else if (di.isSideSecondOnly())
	{
		// right-only
		if (is_dir)
			return (m_nDirs < 3) ? RESULT_RDIRUNIQUE : RESULT_MDIRUNIQUE;
		else
			return (m_nDirs < 3) ? RESULT_RUNIQUE : RESULT_MUNIQUE;
	}
	else if (di.isSideThirdOnly())
	{
		// right-only
		return is_dir ? RESULT_RDIRUNIQUE : RESULT_RUNIQUE;
	}
	else if (m_nDirs > 2)
	{
		switch (diffcode & DIFFCODE::ALL)
		{
		case (DIFFCODE::SECOND | DIFFCODE::THIRD) : return is_dir ? RESULT_LDIRMISSING : RESULT_LMISSING;
		case (DIFFCODE::FIRST  | DIFFCODE::THIRD) : return is_dir ? RESULT_MDIRMISSING : RESULT_MMISSING;
		case (DIFFCODE::FIRST  | DIFFCODE::SECOND): return is_dir ? RESULT_RDIRMISSING : RESULT_RMISSING;
		}
	}
	if (di.isResultError())
	{
		// could be directory error ?
		return RESULT_ERROR;
	}
	// Now we know it was on both sides & compared!
	else if (di.isResultSame())
	{
		// same
		if (is_dir)
		{
			return RESULT_DIRSAME;
		}
		else
		{
			return di.isBin() ? RESULT_BINSAME : RESULT_SAME;
		}
	}
	else
	{
		if (is_dir)
		{
			return RESULT_DIRDIFF;
		}
		else
		{
			// presumably it is different
			return di.isBin() ? RESULT_BINDIFF : RESULT_DIFF;
		}
	}
}

void CompareStats::Swap(int idx1, int idx2)
{
	idx2 = m_nDirs < 3 ? idx2 + 1 : idx2;
	m_counts[RESULT_LUNIQUE     + idx2] = m_counts[RESULT_LUNIQUE     + idx1].exchange(m_counts[RESULT_LUNIQUE     + idx2]);
	m_counts[RESULT_LMISSING    + idx2] = m_counts[RESULT_LMISSING    + idx1].exchange(m_counts[RESULT_LMISSING    + idx2]);
	m_counts[RESULT_LDIRUNIQUE  + idx2] = m_counts[RESULT_LDIRUNIQUE  + idx1].exchange(m_counts[RESULT_LDIRUNIQUE  + idx2]);
	m_counts[RESULT_LDIRMISSING + idx2] = m_counts[RESULT_LDIRMISSING + idx1].exchange(m_counts[RESULT_LDIRMISSING + idx2]);
}
