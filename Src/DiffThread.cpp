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
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "UnicodeString.h"
#include "diffcontext.h"
#include "diffthread.h"
#include "DirScan.h"
#include "Plugins.h"
#include "DiffItemList.h"
#include "PathContext.h"
#include "CompareStats.h"
#include "IAbortable.h"

/**
 * @brief Force compare to be single-threaded.
 * Set this to true in order to single step through entire compare process all
 * in a single thread. Either edit this line, or breakpoint & change it in
 * CompareDirectories() below.
 *
 * If you are going to debug compare procedure, you most probably need to set
 * this to true. As Visual Studio seems to have real problems with debugging
 * these threads otherwise.
 */
static bool bSinglethreaded = false;

/** @brief abort handler for CDiffThread -- just a gateway to CDiffThread */
class DiffThreadAbortable : public IAbortable
{
// Implement DirScan's IAbortable
public:
	virtual bool ShouldAbort() const { return m_diffthread->ShouldAbort(); }

// All this object does is forward ShouldAbort calls to its containing CDiffThread

	DiffThreadAbortable(CDiffThread * diffthread) : m_diffthread(diffthread) { }
	CDiffThread * m_diffthread;
};

/**
 * @brief Default constructor.
 */
CDiffThread::CDiffThread()
: m_pDiffContext(NULL)
, m_msgUpdateUI(0)
, m_hWnd(0)
, m_bAborting(FALSE)
{
	ZeroMemory(&m_threads[0], sizeof(m_threads));
	m_pDiffParm = new DiffFuncStruct;
	m_pAbortgate = new DiffThreadAbortable(this);
}

/**
 * @brief Destructor, release resources.
 */
CDiffThread::~CDiffThread()
{
	CloseHandle(m_pDiffParm->hSemaphore);
	delete m_pDiffParm;
	delete m_pAbortgate;
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
	if (bSinglethreaded)
	{
		MSG msg;
		while (::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
		{
			AfxGetApp()->PumpMessage();
		}
	}
	return m_bAborting;
}

/**
 * @brief Start and run directory compare thread.
 * @param [in] dir1 First directory to compare.
 * @param [in] dir2 Second directory to compare.
 * @return Success (1) or error for thread. Currently always 1.
 */
UINT CDiffThread::CompareDirectories(const String & dir1,
		const String & dir2)
{
	ASSERT(m_pDiffParm->nThreadState != THREAD_COMPARING);

	m_pDiffParm->context = m_pDiffContext;
	m_pDiffParm->msgUIUpdate = m_msgUpdateUI;
	m_pDiffParm->hWindow = m_hWnd;
	m_pDiffParm->m_pAbortgate = m_pAbortgate;
	m_pDiffParm->bOnlyRequested = m_bOnlyRequested;
	m_bAborting = FALSE;

	m_pDiffParm->nThreadState = THREAD_COMPARING;

	m_pDiffParm->hSemaphore = CreateSemaphore(0, 0, LONG_MAX, 0);

	m_pDiffParm->context->m_pCompareStats->SetCompareState(CompareStats::STATE_START);

	if (bSinglethreaded)
	{
		if (m_bOnlyRequested == FALSE)
			DiffThreadCollect(m_pDiffParm);
		DiffThreadCompare(m_pDiffParm);
	}
	else
	{
		if (m_bOnlyRequested == FALSE)
			m_threads[0] = AfxBeginThread(DiffThreadCollect, m_pDiffParm);
		m_threads[1] = AfxBeginThread(DiffThreadCompare, m_pDiffParm);
	}

	return 1;
}

/**
 * @brief Set window receiving messages thread sends.
 * @param [in] hWnd Hand to window to receive messages.
 */
void CDiffThread::SetHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

/**
 * @brief Set message-id for update message.
 * @param [in] updateMsg Message-id for update message.
 */
void CDiffThread::SetMessageIDs(UINT updateMsg)
{
	m_msgUpdateUI = updateMsg;
}

/**
 * @brief Selects to compare all or only selected items.
 * @param [in] bSelected If TRUE only selected items are compared.
 */
void CDiffThread::SetCompareSelected(bool bSelected /*=FALSE*/)
{
	m_bOnlyRequested = bSelected;
}

/**
 * @brief Returns thread's current state
 */
UINT CDiffThread::GetThreadState() const
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
UINT DiffThreadCollect(LPVOID lpParam)
{
	PathContext paths;
	DiffFuncStruct *myStruct = (DiffFuncStruct *) lpParam;

	ASSERT(myStruct->bOnlyRequested == FALSE);

	// Stash abortable interface into context
	myStruct->context->SetAbortable(myStruct->m_pAbortgate);

	// keep the scripts alive during the Rescan
	// when we exit the thread, we delete this and release the scripts
	CAssureScriptsForThread scriptsForRescan;

	bool casesensitive = false;
	int depth = myStruct->context->m_bRecursive ? -1 : 0;

	paths.SetLeft(myStruct->context->GetNormalizedLeft().c_str());
	paths.SetRight(myStruct->context->GetNormalizedRight().c_str());

	String subdir; // blank to start at roots specified in diff context
#ifdef _DEBUG
	_CrtMemState memStateBefore;
	_CrtMemState memStateAfter;
	_CrtMemState memStateDiff;
	_CrtMemCheckpoint(&memStateBefore);
#endif

	// Build results list (except delaying file comparisons until below)
	DirScan_GetItems(paths, subdir, false, subdir, false, myStruct,
			casesensitive, depth, NULL, myStruct->context->m_bWalkUniques);

#ifdef _DEBUG
	_CrtMemCheckpoint(&memStateAfter);
	_CrtMemDifference(&memStateDiff, &memStateBefore, &memStateAfter);
	_CrtMemDumpStatistics(&memStateDiff);
#endif

	// ReleaseSemaphore() once again to signal that collect phase is ready
	ReleaseSemaphore(myStruct->hSemaphore, 1, 0);
	return 1;
}

/**
 * @brief Folder compare thread function.
 *
 * Compares items in item list. After compare is ready
 * sends message to UI so UI can update itself.
 * @param [in] lpParam Pointer to parameter structure.
 * @return Thread's return value.
 */
UINT DiffThreadCompare(LPVOID lpParam)
{
	DiffFuncStruct *myStruct = (DiffFuncStruct *) lpParam;

	// Stash abortable interface into context
	myStruct->context->SetAbortable(myStruct->m_pAbortgate);

	// keep the scripts alive during the Rescan
	// when we exit the thread, we delete this and release the scripts
	CAssureScriptsForThread scriptsForRescan;

	myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_COMPARE);

	// Now do all pending file comparisons
	if (myStruct->bOnlyRequested)
		DirScan_CompareRequestedItems(myStruct, NULL);
	else
		DirScan_CompareItems(myStruct, NULL);

	myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_IDLE);

	// Send message to UI to update
	myStruct->nThreadState = CDiffThread::THREAD_COMPLETED;
	// msgID=MSG_UI_UPDATE=1025 (2005-11-29, Perry)
	PostMessage(myStruct->hWindow, myStruct->msgUIUpdate, NULL, NULL);
	return 1;
}
