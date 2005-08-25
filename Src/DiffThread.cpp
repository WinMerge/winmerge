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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "diffcontext.h"
#include "diffthread.h"
#include "diff.h"
#include "DirScan.h"
#include "Plugins.h"
#include "DiffItemList.h"
#include "PathContext.h"
#include "CompareStats.h"
#include "IAbortable.h"

// Set this to true in order to single step
// through entire compare process all in a single thread
// Either edit this line, or breakpoint & change it in CompareDirectories() below
static bool bSinglethreaded=false;


/**
 * @brief Data sent to diff thread
 */
struct DiffFuncStruct
{
	CString path1;
	CString path2;
	CDiffContext * context;
	UINT msgUIUpdate;
	UINT msgStatusUpdate;
	HWND hWindow;
	UINT nThreadState;
	BOOL bRecursive;
	DiffThreadAbortable * m_pAbortgate;
	bool bOnlyRequested;
	DiffFuncStruct()
		: context(0)
		, msgUIUpdate(0)
		, msgStatusUpdate(0)
		, hWindow(0)
		, nThreadState(THREAD_NOTSTARTED)
		, bRecursive(FALSE)
		, m_pAbortgate(0)
		, bOnlyRequested(false)
		{}
};


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
 * @brief Default constructor
 */
CDiffThread::CDiffThread()
{
	m_pDiffContext = NULL;
	m_thread = NULL;
	m_pDiffParm = new DiffFuncStruct;
	m_pAbortgate = new DiffThreadAbortable(this);
	m_msgUpdateUI = 0;
	m_msgUpdateStatus = 0;
	m_hWnd = 0;
	m_bAborting = FALSE;
}

CDiffThread::~CDiffThread()
{
	delete m_pDiffParm;
	delete m_pAbortgate;

}

/**
 * @brief Sets context pointer forwarded to thread
 */
CDiffContext * CDiffThread::SetContext(CDiffContext * pCtx)
{
	CDiffContext *pTempContext = m_pDiffContext;
	m_pDiffContext = pCtx;
	return pTempContext;
}

/**
 * @brief Start directory compare thread
 */
UINT CDiffThread::CompareDirectories(CString dir1, CString dir2, BOOL bRecursive)
{
	ASSERT(m_pDiffParm->nThreadState != THREAD_COMPARING);

	m_pDiffParm->path1 = dir1;
	m_pDiffParm->path2 = dir2;
	m_pDiffParm->bRecursive = bRecursive;
	m_pDiffParm->context = m_pDiffContext;
	m_pDiffParm->msgUIUpdate = m_msgUpdateUI;
	m_pDiffParm->hWindow = m_hWnd;
	m_pDiffParm->m_pAbortgate = m_pAbortgate;
	m_pDiffParm->bOnlyRequested = m_bOnlyRequested;
	m_bAborting = FALSE;

	m_pDiffParm->nThreadState = THREAD_COMPARING;

	if (bSinglethreaded)
	{
		DiffThread(m_pDiffParm);
	}
	else
	{
		m_thread = AfxBeginThread(DiffThread, m_pDiffParm);
	}

	return 1;
}

/**
 * @brief Set window receiving messages thread sends
 */
void CDiffThread::SetHwnd(HWND hWnd)
{
	m_hWnd = hWnd;
}

/**
 * @brief Set message-id and -number for messages thread sends to window
 */
void CDiffThread::SetMessageIDs(UINT updateMsg, UINT statusMsg)
{
	m_msgUpdateUI = updateMsg;
	m_msgUpdateStatus = statusMsg;
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
 * @brief Directory compare thread function
 *
 * Calls diffutils's compare_files() and after compare is ready
 * sends message to UI so UI can update itself.
 */
UINT DiffThread(LPVOID lpParam)
{
	DiffItemList itemList;
	PathContext paths;
	DiffFuncStruct *myStruct = (DiffFuncStruct *) lpParam;
	HWND hWnd = myStruct->hWindow;
	UINT msgID = myStruct->msgUIUpdate;
	bool bOnlyRequested = myStruct->bOnlyRequested;

	// Stash abortable interface into context
	myStruct->context->SetAbortable(myStruct->m_pAbortgate);

	// keep the scripts alive during the Rescan
	// when we exit the thread, we delete this and release the scripts
	CAssureScriptsForThread scriptsForRescan;

	bool casesensitive = false;
	int depth = myStruct->bRecursive ? -1 : 0;

	paths.SetLeft(myStruct->context->GetNormalizedLeft());
	paths.SetRight(myStruct->context->GetNormalizedRight());
	if (bOnlyRequested)
	{
		myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_COMPARE);
		DirScan_CompareItems(myStruct->context);
		myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_IDLE);
	}
	else
	{
		myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_COLLECT);
		CString subdir; // blank to start at roots specified in diff context
#ifdef _DEBUG
		_CrtMemState memStateBefore;
		_CrtMemState memStateAfter;
		_CrtMemState memStateDiff;
		_CrtMemCheckpoint(&memStateBefore);
#endif

		// Build resultes list (except delaying file comparisons until below)
		DirScan_GetItems(paths, subdir, subdir, &itemList, casesensitive, depth,  myStruct->context);

#ifdef _DEBUG
		_CrtMemCheckpoint(&memStateAfter);
		_CrtMemDifference(&memStateDiff, &memStateBefore, &memStateAfter);
		_CrtMemDumpStatistics(&memStateDiff);
#endif
		myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_COMPARE);

		// Now do all pending file comparisons
		DirScan_CompareItems(itemList, myStruct->context);

		myStruct->context->m_pCompareStats->SetCompareState(CompareStats::STATE_IDLE);
	}

	// Send message to UI to update
	myStruct->nThreadState = THREAD_COMPLETED;
	PostMessage(hWnd, msgID, NULL, NULL);
	return 1;
}
