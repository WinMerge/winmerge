/////////////////////////////////////////////////////////////////////////////
//    WaitStatusCursur
//    Copyright (C) 2003  Perry Rapp
//    Author: Perry Rapp
//
//    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//    You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// WaitStatusCursur.cpp: implementation of the WaitStatusCursur class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaitStatusCursor.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IStatusDisplay * CustomStatusCursor::c_piStatusDisplay = 0;

void CustomStatusCursor::SetStatusDisplay(IStatusDisplay * piStatusDisplay)
{
	c_piStatusDisplay = piStatusDisplay;
}


CStringArray CustomStatusCursor::msgStack;
CPtrArray CustomStatusCursor::cursorStack;
CDWordArray CustomStatusCursor::validStack;
int CustomStatusCursor::stackSize = 0;

CustomStatusCursor::CustomStatusCursor()
: m_ended(false)
{
};

CustomStatusCursor::CustomStatusCursor(HINSTANCE hinst, LPCTSTR lpCursorName, LPCTSTR fmt, ...)
: m_ended(false)
{
	va_list argp;
	va_start(argp, fmt);
	Create(hinst, lpCursorName, fmt, argp);
	va_end(argp);
}

void CustomStatusCursor::Create(HINSTANCE hinst, LPCTSTR lpCursorName, LPCTSTR fmt, ...)
{
	CString m_msg;
	CString m_oldmsg;
	HCURSOR m_prevCursor = NULL;
	HCURSOR m_myCursor = NULL;

	// update status text
	va_list args;
	va_start(args, fmt);
	m_msg.FormatV(fmt, args);
	va_end(args);

	if (c_piStatusDisplay)
		m_oldmsg = c_piStatusDisplay->BeginStatus(m_msg);

	// update cursor
	m_myCursor = ::LoadCursor(hinst, lpCursorName);

	if (m_myCursor)
	{
		m_prevCursor = ::SetCursor(m_myCursor);
	}

//	EnterStackCriticalSection();

	// push info onto the stack
	ASSERT (stackSize == cursorStack.GetSize());
	ASSERT (stackSize == msgStack.GetSize());
	ASSERT (stackSize == validStack.GetSize());
	if (stackSize == 0)
	{
		cursorStack.Add(m_prevCursor);
		msgStack.Add(m_oldmsg);
		validStack.Add(1);
		stackSize ++;
	}
	cursorStack.Add(m_myCursor);
	msgStack.Add(m_msg);
	validStack.Add(1);
	stackSize ++;

	// remember position in stack
	posInStack = cursorStack.GetUpperBound();

//	LeaveStackCriticalSection();
}

CustomStatusCursor::~CustomStatusCursor()
{
	End();
}

void CustomStatusCursor::End()
{

	if (m_ended)
		return;

	m_ended = true;

//	EnterStackCriticalSection();

	ASSERT (stackSize == cursorStack.GetSize());
	ASSERT (stackSize == msgStack.GetSize());
	ASSERT (stackSize == validStack.GetSize());

	// cancel this wait cursor in stack
	validStack.SetAt(posInStack, 0);

	// restore if this wait cursor is on top of stack 
	if (posInStack == cursorStack.GetUpperBound())
	{
		// find the last valid cursor
		int posValid = posInStack-1;
		while (validStack.GetAt(posValid) == 0)
			posValid --;
		ASSERT (posValid >= 0);

		// restore text and cursor
		if (c_piStatusDisplay)
			c_piStatusDisplay->EndStatus(msgStack.GetAt(posInStack), msgStack.GetAt(posValid));

		::SetCursor((HICON__ *) cursorStack.GetAt(posValid));

		// delete all invalid entries from top of stack
		cursorStack.RemoveAt(posValid+1, posInStack - posValid);
		msgStack.RemoveAt(posValid+1, posInStack - posValid);
		validStack.RemoveAt(posValid+1, posInStack - posValid);
		stackSize = posValid+1;

		if (stackSize == 1)
		{
			cursorStack.RemoveAt(0);
			msgStack.RemoveAt(0);
			validStack.RemoveAt(0);
			stackSize --;
		}
	}

//	LeaveStackCriticalSection();
}

/**
 * @brief Send new UI message through GUI feedback, if available
 */
void CustomStatusCursor::ChangeMsg(LPCTSTR fmt, ...)
{
	CString m_msg;
	va_list args;
	va_start(args, fmt);
	m_msg.FormatV(fmt, args);
	va_end(args);

//	EnterStackCriticalSection();

	msgStack.SetAt(posInStack, m_msg);

	// change status only if this wait cursor is on top of stack
	if (posInStack == cursorStack.GetUpperBound())
	{
		if (c_piStatusDisplay)
			c_piStatusDisplay->ChangeStatus(m_msg);
	}

//	LeaveStackCriticalSection();
}


BOOL CustomStatusCursor::HasWaitCursor()
{
//	if (!TryEnterStackCriticalSection())
//		return FALSE;

	ASSERT (stackSize == cursorStack.GetSize());
	ASSERT (stackSize == msgStack.GetSize());
	ASSERT (stackSize == validStack.GetSize());
	BOOL ret = (stackSize >= 2);

//	LeaveStackCriticalSection();

	return ret;
}

/**
 * @brief Same interface as CCmdTarget::RestoreWaitCursor
 */
void CustomStatusCursor::RestoreWaitCursor()
{
//	if (!TryEnterStackCriticalSection())
//		return;

	ASSERT (stackSize == cursorStack.GetSize());
	ASSERT (stackSize == msgStack.GetSize());
	ASSERT (stackSize == validStack.GetSize());

	// the top of stack is always valid
	int posValid = stackSize-1;
	ASSERT(validStack.GetAt(posValid));

	// restore text and cursor
	if (c_piStatusDisplay)
		c_piStatusDisplay->ChangeStatus(msgStack.GetAt(posValid));

	::SetCursor((HICON__ *) cursorStack.GetAt(posValid));

//	LeaveStackCriticalSection();
}


/*
CRITICAL_SECTION CustomStatusCursor::CriticalSectionStack;
BOOL CustomStatusCursor::bCriticalSectionExist = FALSE;
/// Flag to implement TryEnterStackCriticalSection (TryEnterCriticalSection not available in w95/98)
BOOL CustomStatusCursor::bCriticalSectionEntered = FALSE;


void CustomStatusCursor::EnterStackCriticalSection()
{
	// initialize specific resource if necessary
	if (!bCriticalSectionExist)
	{
		InitializeCriticalSection(&CriticalSectionStack);
		bCriticalSectionExist = TRUE;
	}

	// lock specific resource
	EnterCriticalSection(&CriticalSectionStack);
	bCriticalSectionEntered = TRUE;
}
void CustomStatusCursor::LeaveStackCriticalSection()
{
	LeaveCriticalSection(&CriticalSectionStack);
	bCriticalSectionEntered = FALSE;
}

BOOL CustomStatusCursor::TryEnterStackCriticalSection()
{
	// Test the flag first
	if (bCriticalSectionEntered)
		return FALSE;

	EnterStackCriticalSection();
	return TRUE;
}
*/

WaitStatusCursor::WaitStatusCursor(LPCTSTR fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	Create(NULL, IDC_WAIT, fmt, argp);
	va_end(argp);
}

WaitStatusCursor::WaitStatusCursor(UINT fmtid, ...)
{
	va_list argp;
	va_start(argp, fmtid);
	Create(NULL, IDC_WAIT, theApp.LoadString(fmtid).c_str(), argp);
	va_end(argp);
}
