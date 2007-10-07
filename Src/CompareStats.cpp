/** 
 * @file  CompareStats.cpp
 *
 * @brief Implementation of CompareStats class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "DiffItem.h"
#include "CompareStats.h"

/** 
 * @brief Constructor, initializes critical section.
 */
CompareStats::CompareStats()
: m_nTotalItems(0)
, m_nComparedItems(0)
, m_state(STATE_IDLE)
, m_bCompareDone(FALSE)
{
	InitializeCriticalSection(&m_csProtect);
	ZeroMemory(&m_counts[0], sizeof(m_counts));
}

/** 
 * @brief Destructor, deletes critical section.
 */
CompareStats::~CompareStats()
{
	DeleteCriticalSection(&m_csProtect);
}

/** 
 * @brief Increase found items (dirs and files) count.
 * @param [in] count Amount of items to add.
 */
void CompareStats::IncreaseTotalItems(int count /*= 1*/)
{
	EnterCriticalSection(&m_csProtect);
	m_nTotalItems += count;
	LeaveCriticalSection(&m_csProtect);
}

/** 
 * @brief Add compared item.
 * @param [in] code Resultcode to add.
 */
void CompareStats::AddItem(int code)
{
	EnterCriticalSection(&m_csProtect);
	RESULT res = GetResultFromCode(code);
	int index = static_cast<int>(res);
	m_counts[index] += 1;
	++m_nComparedItems;
	ASSERT(m_nComparedItems <= m_nTotalItems);
	LeaveCriticalSection(&m_csProtect);
}

/** 
 * @brief Return count by resultcode.
 * @param [in] result Resultcode to return.
 * @return Count of items for given resultcode.
 */
int CompareStats::GetCount(CompareStats::RESULT result) const
{
	// What is the point in doing thread synchronization here?
	return m_counts[result];
	/*int currentValue = 0;
	EnterCriticalSection(&m_csProtect);
	int resInd = static_cast<int>(result);
	currentValue = m_counts[resInd];
	LeaveCriticalSection(&m_csProtect);
	return currentValue;*/
}

/** 
 * @brief Return total count of items (so far) found.
 */
int CompareStats::GetTotalItems() const
{
	return m_nTotalItems;
}

/** 
 * @brief Reset comparestats.
 * Use this function to reset stats before new compare.
 */
void CompareStats::Reset()
{
	ZeroMemory(&m_counts[0], sizeof(m_counts));
	SetCompareState(STATE_IDLE);
	m_nTotalItems = 0;
	m_nComparedItems = 0;
	m_bCompareDone = FALSE;
}

/** 
 * @brief Change compare state.
 * @param [in] state New compare state.
 */
void CompareStats::SetCompareState(CompareStats::CMP_STATE state)
{
#ifdef _DEBUG
	if (state == STATE_START && m_state != STATE_IDLE)
		_RPTF2(_CRT_ERROR, "Invalid state change from %d to %d", m_state, state);
#endif //_DEBUG

	// New compare starting so reset ready status
	if (state == STATE_START)
		m_bCompareDone = FALSE;
	// Compare ready
	if (state == STATE_IDLE && m_state == STATE_COMPARE)
		m_bCompareDone = TRUE;

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
CompareStats::RESULT CompareStats::GetResultFromCode(UINT diffcode)
{
	DIFFCODE di = diffcode;
	
	// Test first for skipped so we pick all skipped items as such 
	if (di.isResultFiltered())
	{
		// skipped
		if (di.isDirectory())
		{
			return RESULT_DIRSKIP;
		}
		else
		{
			return RESULT_SKIP;
		}
	}
	else if (di.isSideLeftOnly())
	{
		// left-only
		if (di.isDirectory())
		{
			return RESULT_LDIRUNIQUE;
		}
		else
		{
			return RESULT_LUNIQUE;
		}
	}
	else if (di.isSideRightOnly())
	{
		// right-only
		if (di.isDirectory())
		{
			return RESULT_RDIRUNIQUE;
		}
		else
		{
			return RESULT_RUNIQUE;
		}
	}
	else if (di.isResultError())
	{
		// could be directory error ?
		return RESULT_ERROR;
	}
	// Now we know it was on both sides & compared!
	else if (di.isResultSame())
	{
		// same
		if (di.isBin())
		{
			return RESULT_BINSAME;
		}
		else
		{
			return RESULT_SAME;
		}
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
			if (di.isBin())
			{
				return RESULT_BINDIFF;
			}
			else
			{
				return RESULT_DIFF;
			}
		}
	}
}
