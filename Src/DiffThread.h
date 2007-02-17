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
 * @brief Thread's statuses.
 */
enum
{
	THREAD_NOTSTARTED = 0,
	THREAD_COMPARING,
	THREAD_COMPLETED,
};


/**
 * @brief Class for threaded folder compare.
 * This class implements folder compare in two phases and in two threads:
 * - first thread collects items to compare to compare-time list
 *   (m_diffList).
 * - second threads compares items in the list.
 */
class CDiffThread
{
public:
// creation and use, called on main thread
	CDiffThread();
	~CDiffThread();
	void SetContext(CDiffContext * pCtx);
	UINT CompareDirectories(const CString & dir1, const CString & dir2, BOOL bRecursive);
	void SetHwnd(HWND hWnd);
	void SetMessageIDs(UINT updateMsg, UINT statusMsg);
	void SetCompareSelected(bool bSelected = false);

// runtime interface for main thread, called on main thread
	UINT GetThreadState() const;
	void Abort() { m_bAborting = true; }
	bool IsAborting() const { return m_bAborting; }

// runtime interface for child thread, called on child thread
	bool ShouldAbort() const { return m_bAborting; }

private:
	CDiffContext * m_pDiffContext; /**< Compare context storing results. */
	CWinThread * m_threads[2]; /**< Compare threads. */
	DiffFuncStruct * m_pDiffParm; /**< Structure for sending data to threads. */
	DiffThreadAbortable * m_pAbortgate;
	DiffItemList m_diffList; /**< Compare-time list for compared items. */
	UINT m_msgUpdateUI; /**< UI-update message number */
	HWND m_hWnd; /**< Handle to folder compare GUI window */
	bool m_bAborting; /**< Is compare aborting? */
	bool m_bOnlyRequested; /**< Are we comparing only requested items (Update?) */
};

// Thread functions
UINT DiffThreadCollect(LPVOID lpParam);
UINT DiffThreadCompare(LPVOID lpParam);

#endif /* _DIFFTHREAD_H */