/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  AnsiConvert.cpp
 *
 * @brief Implementation of the AnsiConvert class.
 *
 */
#include "precomp.h"
#include "AnsiConvert.h"

MakeAnsi::MakeAnsi(PCWSTR text, UINT codepage, int textlen)
: m_bstr(0)
{
	if (text)
	{
		int len = WideCharToMultiByte(codepage, 0, text, textlen, 0, 0, 0, 0);
		if (len)
		{
			m_bstr = SysAllocStringByteLen(0, len - 1);
			WideCharToMultiByte(codepage, 0, text, textlen, (PSTR)m_bstr, len, 0, 0);
		}
	}
}

MakeWide::MakeWide(PCSTR text, UINT codepage, int textlen)
: m_bstr(0)
{
	if (text)
	{
		int len = MultiByteToWideChar(codepage, 0, text, textlen, 0, 0);
		if (len)
		{
			m_bstr = SysAllocStringLen(0, len - 1);
			MultiByteToWideChar(codepage, 0, text, textlen, m_bstr, len);
		}
	}
}
