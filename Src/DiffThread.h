/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffThread.h
 *
 * @brief Declaration file for CDiffThread
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _DIFFTTHREAD_H
#define _DIFFTHREAD_H

#include "diffcontext.h"

struct DiffFuncStruct;
class DiffThreadAbortable;

/**
 * @brief Thread's statuses
 */
enum
{
	THREAD_NOTSTARTED = 0,
	THREAD_COMPARING,
	THREAD_COMPLETED
};


/**
 * @brief Class for threaded directory compare
 * This class takes care of starting directory compare thread
 */
class CDiffThread
{
public:
// creation and use, called on main thread
	CDiffThread();
	~CDiffThread();
	CDiffContext * SetContext(CDiffContext * pCtx);
	UINT CompareDirectories(CString dir1, CString dir2,	BOOL bRecursive);
	void SetHwnd(HWND hWnd);
	void SetMessageIDs(UINT updateMsg, UINT statusMsg);

// runtime interface for main thread, called on main thread
	UINT GetThreadState() const;
	void Abort() { m_bAborting = true; }
	bool IsAborting() const { return m_bAborting; }

// runtime interface for child thread, called on child thread
	bool ShouldAbort() const { return m_bAborting; }


private:
	CDiffContext * m_pDiffContext;
	CWinThread * m_thread;
	DiffFuncStruct * m_pDiffParm;
	DiffThreadAbortable * m_pAbortgate;
	UINT m_msgUpdateUI;
	UINT m_msgUpdateStatus;
	HWND m_hWnd;
	bool m_bAborting;
};

UINT DiffThread(LPVOID lpParam);

#endif /* _DIFFTHREAD_H */