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
 * @file  AnsiConvert.cpp
 *
 * @brief Implementation of the AnsiConvert class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: AnsiConvert.cpp 0 0000-00-00 00:00:00Z jtuc $
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
