/*!
  \file    CSubclass.cpp
  \author  Perry Rapp, Creator, 1998-2003
  \date    Created: 1998
  \date    Edited:  2001/12/12 PR

  \brief   Implementation of CSubclass

*/
/* The MIT License
Copyright (c) 2001 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "StdAfx.h"
#include "CSubclass.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace prdlg {

struct SubclassRec
{
	WNDPROC m_newproc;
	WNDPROC m_oldproc;
	void * m_data;
	bool m_suppressing; // set during the failed message
	Msg m_succeededMsg;
	Msg m_failedMsg;
};

// all the subclasses for a given hwnd
// newest is head
typedef CList<SubclassRec, SubclassRec &> SubclassList;

// map of hwnds to subclasses
typedef CTypedPtrMap<CMapPtrToPtr, HWND, SubclassList*> SubclassMap;

#ifdef _MT
#define THREAD _declspec(thread)
#else
#define THREAD
#endif
// map of hwnds to subclasses for current thread
static THREAD SubclassMap * f_Subclasses=0;



bool Subclass(WNDPROC wndproc, HWND hwnd, void * data
	, Msg * unsubclassSucceeded, Msg * unsubclassFailed)
{
		ASSERT(IsWindow(hwnd));
		// cross-thread window usage is not kosher ?
		ASSERT(GetCurrentThreadId() == GetWindowThreadProcessId(hwnd, 0));
	if (!f_Subclasses)
		f_Subclasses = new SubclassMap;
	SubclassList *pList = 0;
	if (f_Subclasses->Lookup(hwnd, pList))
	{ // we've already subclassed this window
		for (POSITION pos = pList->GetHeadPosition(); pos; pList->GetNext(pos))
		{
			SubclassRec & srec = pList->GetAt(pos);
			if (srec.m_newproc == wndproc)
				return false; // cannot resubclass with same WNDPROC
		}
	}
	else
	{
		pList = new SubclassList;
		f_Subclasses->SetAt(hwnd, pList);
	}
	SubclassRec sdata;
	sdata.m_data = data;
	sdata.m_newproc = wndproc;
	sdata.m_oldproc = (WNDPROC) GetWindowLong(hwnd, GWL_WNDPROC);
	sdata.m_suppressing = false;
	if (unsubclassSucceeded)
		sdata.m_succeededMsg = *unsubclassSucceeded;
	else
		sdata.m_succeededMsg.msg = 0;
	if (unsubclassFailed)
		sdata.m_failedMsg = *unsubclassFailed;
	else
		sdata.m_failedMsg.msg = 0;
	SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM)(WNDPROC)wndproc);
	pList->AddHead(sdata);
	return true;
}

bool UnSubclass(WNDPROC id, HWND hwnd)
{
		ASSERT(IsWindow(hwnd));
		// cross-thread window usage is not kosher ?
		ASSERT(GetCurrentThreadId() == GetWindowThreadProcessId(hwnd, 0));
	SubclassList *pList = 0;
		if (!f_Subclasses || !f_Subclasses->Lookup(hwnd, pList))
			// We've not subclassed this hwnd!
			return false;
		if (!pList->GetCount())
			// internal error
			return false;
	POSITION pos = pList->GetHeadPosition();
	SubclassRec srec = pList->GetNext(pos);
	if (srec.m_newproc == id)
	{ // we're the latest subclass
			WNDPROC curproc = (WNDPROC) GetWindowLong(hwnd, GWL_WNDPROC);
			if (id != curproc)
				// We're not the current wndproc, so we can't safely unhook
				return false;
			ASSERT(srec.m_oldproc); // internal error
		SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM)(WNDPROC)srec.m_oldproc);
		pList->RemoveHead();
		// fall thru to garbage collect list
	}
	else
	{ // we're not the latest, but try to unhook anyway (may be ok)
		bool found=false;
		SubclassRec sprevrec = srec;
		for ( ; pos; pList->GetNext(pos), sprevrec=srec)
		{
			srec = pList->GetAt(pos);
			if (srec.m_newproc == id)
			{
				found=true;
				break;
			}
		}
			if (!found)
				// We've not subclassed with this wndproc (id) (or its been unsubclassed already)
				return false;
			if (sprevrec.m_oldproc != id)
				// Foreign subclass has intervened - we can't unhook
				return false;
		sprevrec.m_oldproc = srec.m_oldproc;
		pList->RemoveAt(pos);
		// fall thru to garbage collect list
	}
	// garbage collect
	if (!pList->GetCount())
	{
		f_Subclasses->RemoveKey(hwnd);
		delete pList;
		if (!f_Subclasses->GetCount())
		{
			delete f_Subclasses;
			f_Subclasses = 0;
		}
	}
	return true;
}

static SubclassRec * FindSubclass(WNDPROC id, HWND hwnd)
{
	SubclassList *pList = 0;
		if (!f_Subclasses || !f_Subclasses->Lookup(hwnd, pList))
			// We've not subclassed this hwnd!
			return 0;
	for (POSITION pos = pList->GetHeadPosition(); pos; pList->GetNext(pos))
	{
		SubclassRec & srec = pList->GetAt(pos);
		if (srec.m_newproc == id)
			return &srec;
	}
	// We couldn't find our subclass of this hwnd
	return 0;
}

LRESULT CallOldProc(WNDPROC id, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	SubclassRec * psrec = FindSubclass(id, hwnd);
		if (!psrec) return 0;
	WNDPROC oldproc = psrec->m_oldproc; // psrec may get destroyed by the UnSubclass
	if (msg == WM_DESTROY || msg == WM_NCDESTROY)
	{
		Msg smsg = psrec->m_succeededMsg;
		bool succeeded = UnSubclass(id, hwnd);
		// if UnSubclass succeeds, psrec will be deleted
		if (succeeded)
			psrec = 0;
		if (!psrec && smsg.msg)
		{
			// succeeded, inform client
			CallWindowProc(id, hwnd, smsg.msg, smsg.wparam, smsg.lparam);
		}
		if (psrec && (msg == WM_NCDESTROY) && psrec->m_failedMsg.msg)
		{
			// failed our last chance, inform client
			Msg & fmsg = psrec->m_failedMsg;
			psrec->m_suppressing = true;
			CallWindowProc(id, hwnd, fmsg.msg, fmsg.wparam, fmsg.lparam);
			psrec->m_suppressing = false;
		}
	}
	// we don't forward the failed message
	// (the succeeded message will never get here, as it was sent after unsubclassed)
	if (psrec && psrec->m_suppressing)
	{
		const Msg & tmsg = psrec->m_failedMsg;
		if (tmsg.msg == msg && tmsg.wparam == wParam && tmsg.lparam == lParam)
			return 0;
	}

	return CallWindowProc(oldproc, hwnd, msg, wParam, lParam);
}

void * GetData(WNDPROC id, HWND hwnd)
{
	SubclassRec * psrec = FindSubclass(id, hwnd);
		if (!psrec) return 0;
	return psrec->m_data;
}




} // namespace
