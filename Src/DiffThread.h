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
};

/**
 * @brief Class for threaded directory compare
 * This class takes care of starting directory compare thread
 */
class CDiffThread
{
public:
	CDiffThread();
	~CDiffThread();
	CDiffContext * SetContext(CDiffContext * pCtx);
	UINT CompareDirectories(CString dir1, CString dir2,	BOOL bRecursive);
	void SetHwnd(HWND hWnd);
	void SetMessageIDs(UINT updateMsg, UINT statusMsg);
	UINT GetThreadState();

private:
	CDiffContext * m_pDiffContext;
	CWinThread * m_thread;
	UINT m_msgUpdateUI;
	UINT m_msgUpdateStatus;
	HWND m_hWnd;
};

UINT DiffThread(LPVOID lpParam);

#endif /* _DIFFTHREAD_H */