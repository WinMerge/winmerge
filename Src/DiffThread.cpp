// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  DiffThread.cpp
 *
 * @brief Code for DiffThread class
 */

#include "pch.h"
#include "DiffThread.h"
#include <cassert>
#include <climits>
#include <Poco/Thread.h>
#include <Poco/Semaphore.h>
#include "UnicodeString.h"
#include "CompareStats.h"
#include "IAbortable.h"
#include "Plugins.h"
#include "MergeAppCOMClass.h"
#include "DebugNew.h"

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
	virtual bool ShouldAbort() const override { return m_diffthread->ShouldAbort(); }

// All this object does is forward ShouldAbort calls to its containing CDiffThread

	explicit DiffThreadAbortable(CDiffThread * diffthread) : m_diffthread(diffthread) { }
	CDiffThread * m_diffthread;
};

/**
 * @brief Default constructor.
 */
CDiffThread::CDiffThread()
: m_pDiffContext(nullptr)
, m_bAborting(false)
, m_bPaused(false)
, m_pDiffParm(new DiffFuncStruct)
, m_pAbortgate(new DiffThreadAbortable(this))
{
}

/**
 * @brief Destructor, release resources.
 */
CDiffThread::~CDiffThread()
{
	delete m_pDiffParm->pSemaphore;
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
	assert(m_pDiffParm->nThreadState != THREAD_COMPARING && m_pDiffParm->nCollectThreadState != THREAD_COMPARING);

	m_pDiffParm->context = m_pDiffContext;
	m_pDiffParm->m_pAbortgate = m_pAbortgate.get();
	m_bAborting = false;
	m_bPaused = false;

	m_pDiffParm->nThreadState = THREAD_COMPARING;
	m_pDiffParm->nCollectThreadState = THREAD_COMPARING;

	delete m_pDiffParm->pSemaphore;
	m_pDiffParm->pSemaphore = new Semaphore(0, LONG_MAX);

	m_pDiffParm->context->m_pCompareStats->SetCompareState(CompareStats::STATE_START);

	m_threads[0].start(DiffThreadCollect, m_pDiffParm.get());
	m_threads[1].start(DiffThreadCompare, m_pDiffParm.get());

	return 1;
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
	DiffFuncStruct *myStruct = static_cast<DiffFuncStruct *>(pParam);

	// Stash abortable interface into context
	myStruct->context->SetAbortable(myStruct->m_pAbortgate);

	if (myStruct->m_fncCollect)
		myStruct->m_fncCollect(myStruct);

	// Release Semaphore() once again to signal that collect phase is ready
	myStruct->pSemaphore->set();

	// Send message to UI to update
	myStruct->nCollectThreadState = CDiffThread::THREAD_COMPLETED;
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
	CAssureScriptsForThread scriptsForRescan(new MergeAppCOMClass());

	// Stash abortable interface into context
	myStruct->context->SetAbortable(myStruct->m_pAbortgate);

	myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_COMPARE);

	// Now do all pending file comparisons
	myStruct->m_fncCompare(myStruct);

	myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_IDLE);

	// Send message to UI to update
	myStruct->nThreadState = CDiffThread::THREAD_COMPLETED;
	int event = CDiffThread::EVENT_COMPARE_COMPLETED;
	myStruct->m_listeners.notify(myStruct, event);
}
