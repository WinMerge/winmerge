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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IStatusDisplay * WaitStatusCursor::c_piStatusDisplay = 0;


void WaitStatusCursor::SetStatusDisplay(IStatusDisplay * piStatusDisplay)
{
	c_piStatusDisplay = piStatusDisplay;
}


WaitStatusCursor::WaitStatusCursor(LPCTSTR fmt, ...)
: m_ended(false)
{
	va_list args;
	va_start(args, fmt);
	m_msg.FormatV(fmt, args);
	va_end(args);
	if (c_piStatusDisplay)
		m_oldmsg = c_piStatusDisplay->BeginStatus(m_msg);
}

WaitStatusCursor::~WaitStatusCursor()
{
	End();
}

void WaitStatusCursor::End()
{
	if (!m_ended)
	{
		m_ended = true;
		if (c_piStatusDisplay)
			c_piStatusDisplay->EndStatus(m_msg, m_oldmsg);
		m_msg = _T("");
		m_oldmsg = _T("");
	}
}

void WaitStatusCursor::ChangeMsg(LPCTSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	m_msg.FormatV(fmt, args);
	va_end(args);
	if (c_piStatusDisplay)
		c_piStatusDisplay->ChangeStatus(m_msg);
}



