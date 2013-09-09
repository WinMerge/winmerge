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
 * @file  AnsiConvert.h
 *
 * @brief  Class for converting strings from wide char to ANSI.
 *
 */
#ifndef _ANSI_CONVERT_H_
#define _ANSI_CONVERT_H_

/**
 * @brief Convert string from wide char to ANSI.
 */
class MakeAnsi
{
private:
	BSTR m_bstr;
public:
	MakeAnsi(PCWSTR text, UINT codepage = CP_ACP, int textlen = -1);
	MakeAnsi(): m_bstr(0) { }
	~MakeAnsi() { SysFreeString(m_bstr); }
	operator PCSTR() { return (PCSTR)m_bstr; }
	UINT GetLength()
	{
		return SysStringByteLen(m_bstr);
	}
	PSTR GetBufferSetLength(UINT len)
	{
		assert((len & 1) == 0);
		return SysReAllocStringLen(&m_bstr, 0, len / 2) ? PSTR(m_bstr) : 0;
	}
};

/**
 * @brief Convert string from ANSI to wide char.
 */
class MakeWide
{
private:
	BSTR m_bstr;
public:
	MakeWide(PCSTR text, UINT codepage = CP_ACP, int textlen = -1);
	MakeWide(): m_bstr(0) { }
	~MakeWide() { SysFreeString(m_bstr); }
	operator PCWSTR() { return m_bstr; }
	UINT GetLength()
	{
		return SysStringLen(m_bstr);
	}
	PWSTR GetBufferSetLength(UINT len)
	{
		return SysReAllocStringLen(&m_bstr, 0, len) ? m_bstr : 0;
	}
};

typedef MakeWide A2W;
typedef MakeAnsi W2A;
#ifdef UNICODE
typedef PCWSTR T2W, W2T;
typedef MakeAnsi T2A;
typedef MakeWide A2T;
#else
typedef PCSTR T2A, A2T;
typedef MakeWide T2W;
typedef MakeAnsi W2T;
#endif

#endif
