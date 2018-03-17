/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffThread.cpp
 *
 * @brief Code for DiffThread class
 */

#include "DiffThread.h"
#include <cassert>
#include <climits>
#include <Poco/Thread.h>
#include <Poco/Semaphore.h>
#include "UnicodeString.h"
#include "DiffContext.h"
#include "DirScan.h"
#include "DiffItemList.h"
#include "PathContext.h"
#include "CompareStats.h"
#include "IAbortable.h"

using Poco::Thread;
using Poco::Semaphore;

// Thread functions
static void DiffThreadCollect(void *lpParam);
static void DiffThreadCompare(void *lpParam);

/** @brief abort handler for CDiffThread -- just a gateway to CDiffThread */
class DiffThreadAbortable : public IAbortable
{
// Implement DirScan's IAbortable
public:
	virtual bool ShouldAbort() const { return m_diffthread->ShouldAbort(); }

// All this object does is forward ShouldAbort calls to its containing CDiffThread

	explicit DiffThreadAbortable(CDiffThread * diffthread) : m_diffthread(diffthread) { }
	CDiffThread * m_diffthread;
};

/**
 * @brief Default constructor.
 */
CDiffThread::CDiffThread()
: m_pDiffContext(NULL)
, m_bAborting(false)
, m_bPaused(false)
, m_pDiffParm(new DiffFuncStruct)
{
	m_pAbortgate.reset(new DiffThreadAbortable(this));
}

/**
 * @brief Destructor, release resources.
 */
CDiffThread::~CDiffThread()
{
	delete m_pDiffParm->pSemaphore;
}

/**
 * @brief Sets context pointer forwarded to thread.
 * @param [in] pCtx Pointer to compare context.
 */
void CDiffThread::SetContext(CDiffContext * pCtx)
{
	m_pDiffContext = pCtx;
}

/**
 * @brief runtime interface for child thread, called on child thread
 */
bool CDiffThread::ShouldAbort() const
{
	while (m_bPaused && !m_bAborting)
		Poco::Thread::sleep(100);
	return m_bAborting;
}

/**
 * @brief Start and run directory compare thread.
 * @return Success (1) or error for thread. Currently always 1.
 */
unsigned CDiffThread::CompareDirectories()
{
	assert(m_pDiffParm->nThreadState != THREAD_COMPARING);

	m_pDiffParm->context = m_pDiffContext;
	m_pDiffParm->m_pAbortgate = m_pAbortgate.get();
	m_pDiffParm->bOnlyRequested = m_bOnlyRequested;
	m_bAborting = false;
	m_bPaused = false;

	m_pDiffParm->nThreadState = THREAD_COMPARING;

	delete m_pDiffParm->pSemaphore;
	m_pDiffParm->pSemaphore = new Semaphore(0, LONG_MAX);

	m_pDiffParm->context->m_pCompareStats->SetCompareState(CompareStats::STATE_START);

	if (m_bOnlyRequested == false)
		m_threads[0].start(DiffThreadCollect, m_pDiffParm.get());
	else
	{
		int nItems = DirScan_UpdateMarkedItems(m_pDiffParm.get(), 0);
		// Send message to UI to update
		int event = CDiffThread::EVENT_COLLECT_COMPLETED;
		m_pDiffParm->m_listeners.notify(m_pDiffParm.get(), event);
		m_pDiffParm->context->m_pCompareStats->IncreaseTotalItems(nItems - m_pDiffParm->context->m_pCompareStats->GetTotalItems());
	}
	m_threads[1].start(DiffThreadCompare, m_pDiffParm.get());

	return 1;
}

/**
 * @brief Selects to compare all or only selected items.
 * @param [in] bSelected If true only selected items are compared.
 */
void CDiffThread::SetCompareSelected(bool bSelected /*=false*/)
{
	m_bOnlyRequested = bSelected;
}

/**
 * @brief Returns thread's current state
 */
unsigned CDiffThread::GetThreadState() const
{
	return m_pDiffParm->nThreadState;
}

/**
 * @brief Item collection thread function.
 *
 * This thread is responsible for finding and collecting all items to compare
 * to the item list.
 * @param [in] lpParam Pointer to parameter structure.
 * @return Thread's return value.
 */
static void DiffThreadCollect(void *pParam)
{
	PathContext paths;
	DiffFuncStruct *myStruct = static_cast<DiffFuncStruct *>(pParam);

	assert(myStruct->bOnlyRequested == false);

	// Stash abortable interface into context
	myStruct->context->SetAbortable(myStruct->m_pAbortgate);

	bool casesensitive = false;
	int depth = myStruct->context->m_bRecursive ? -1 : 0;

	paths = myStruct->context->GetNormalizedPaths();

	String subdir[3] = {_T(""), _T(""), _T("")}; // blank to start at roots specified in diff context

	// Build results list (except delaying file comparisons until below)
	DirScan_GetItems(paths, subdir, myStruct,
			casesensitive, depth, NULL, myStruct->context->m_bWalkUniques);

	// ReleaseSemaphore() once again to signal that collect phase is ready
	myStruct->pSemaphore->set();

	// Send message to UI to update
	int event = CDiffThread::EVENT_COLLECT_COMPLETED;
	myStruct->m_listeners.notify(myStruct, event);
};

/**
 * @brief Folder compare thread function.
 *
 * Compares items in item list. After compare is ready
 * sends message to UI so UI can update itself.
 * @param [in] lpParam Pointer to parameter structure.
 * @return Thread's return value.
 */
static void DiffThreadCompare(void *pParam)
{
	DiffFuncStruct *myStruct = static_cast<DiffFuncStruct *>(pParam);

	// Stash abortable interface into context
	myStruct->context->SetAbortable(myStruct->m_pAbortgate);

	myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_COMPARE);

	// Now do all pending file comparisons
	if (myStruct->bOnlyRequested)
		DirScan_CompareRequestedItems(myStruct, 0);
	else
		DirScan_CompareItems(myStruct, 0);

	myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_IDLE);

	// Send message to UI to update
	myStruct->nThreadState = CDiffThread::THREAD_COMPLETED;
	int event = CDiffThread::EVENT_COMPARE_COMPLETED;
	myStruct->m_listeners.notify(myStruct, event);
}
