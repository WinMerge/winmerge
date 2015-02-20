/////////////////////////////////////////////////////////////////////////////
//    WaitStatusCursur
//    Copyright (C) 2003  Perry Rapp
//    Author: Perry Rapp
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version. This program is distributed in
//    the hope that it will be useful, but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more
//    details. You should have received a copy of the GNU General Public
//    License along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file WaitStatusCursor.cpp
 *
 *  @brief Implementation of the WaitStatusCursur class.
 */ 

#include "stdafx.h"
#include "WaitStatusCursor.h"
#include "CustomStatusCursor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class WaitStatusCursor::Impl : public CustomStatusCursor
{
// public interface
public:
	Impl(const String& fmt);
};

WaitStatusCursor::Impl::Impl(const String& msg)
{
	Create(NULL, IDC_WAIT, msg.c_str());
}

WaitStatusCursor::WaitStatusCursor(const String& fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	String msg = string_format_arg_list(fmt.c_str(), argp);
	va_end(argp);
	m_pimpl.reset(new WaitStatusCursor::Impl(msg));
}

WaitStatusCursor::~WaitStatusCursor()
{
}
