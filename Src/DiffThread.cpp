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

/**
 * @brief Static structure for sharing data with thread
 */
static DiffFuncStruct diffParam;

/**
 * @brief Default constructor
 */
CDiffThread::CDiffThread()
{
	m_pDiffContext = NULL;
	m_thread = NULL;
	diffParam.nThreadState = THREAD_NOTSTARTED;
}

CDiffThread::~CDiffThread()
{

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
	diffParam.path1 = dir1;
	diffParam.path2 = dir2;
	diffParam.bRecursive = bRecursive;
	diffParam.context = m_pDiffContext;
	diffParam.msgUIUpdate = m_msgUpdateUI;
	diffParam.hWindow = m_hWnd;

	diffParam.nThreadState = THREAD_COMPARING;
	m_thread = AfxBeginThread(DiffThread, (LPVOID)&diffParam);
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
UINT CDiffThread::GetThreadState()
{
	return diffParam.nThreadState;
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

	bool casesensitive = false;
	int depth = myStruct->bRecursive ? -1 : 0;
	CString subdir; // blank to start at roots specified in diff context
	DirScan(subdir, myStruct->context, casesensitive, depth);
	
	// Send message to UI to update
	diffParam.nThreadState = THREAD_COMPLETED;
	SendMessage(hWnd, msgID, NULL, NULL);
	return 1;
}
