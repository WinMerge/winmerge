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


/**
 * @brief Data sent to diff thread
 */
struct DiffFuncStruct
{
	bool bSinglethreaded;
	CString path1;
	CString path2;
	CDiffContext * context;
	UINT msgUIUpdate;
	UINT msgStatusUpdate;
	HWND hWindow;
	UINT nThreadState;
	BOOL bRecursive;
	DiffThreadAbortable * m_pAbortgate;
	DiffFuncStruct()
		: bSinglethreaded(false)
		, context(0)
		, msgUIUpdate(0)
		, msgStatusUpdate(0)
		, hWindow(0)
		, nThreadState(THREAD_NOTSTARTED)
		, bRecursive(FALSE)
		, m_pAbortgate(0)
		{}
};


/** @brief abort handler for CDiffThread -- just a gateway to CDiffThread */
class DiffThreadAbortable : public IAbortable
{
// Implement DirScan's IAbortable
public:
	virtual bool ShouldAbort() { return m_diffthread->ShouldAbort(); }

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
	m_bAborting = FALSE;

	m_pDiffParm->nThreadState = THREAD_COMPARING;

	// When interactively debugging, this is a way to force all diff code to run in main thread
	static bool singlethreaded = false;

	if (singlethreaded)
	{
		m_pDiffParm->bSinglethreaded = true;
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
	DiffFuncStruct *myStruct = (DiffFuncStruct *) lpParam;
	HWND hWnd = myStruct->hWindow;
	UINT msgID = myStruct->msgUIUpdate;

	// keep the scripts alive during the Rescan
	// when we exit the thread, we delete this and release the scripts
	CScriptsOfThread * scriptsForRescan = 0;
	if (!myStruct->bSinglethreaded)
		scriptsForRescan = new CScriptsOfThread;

	bool casesensitive = false;
	int depth = myStruct->bRecursive ? -1 : 0;
	CString subdir; // blank to start at roots specified in diff context
	DirScan(subdir, myStruct->context, casesensitive, depth, myStruct->m_pAbortgate);
	
	// Send message to UI to update
	myStruct->nThreadState = THREAD_COMPLETED;
	PostMessage(hWnd, msgID, NULL, NULL);

	if (scriptsForRescan)
		delete scriptsForRescan;
	return 1;
}
