/* File:	tools.cpp
 * Author:	Jochen Tucht 2003/12/09
 *			Copyright (C) Jochen Tucht
 *
 * Purpose:	supplementary classes and functions for Merge7z
 *
 * Remarks:	
 *
 *	*** SECURITY ALERT ***
 *	Be aware of 2. a) of the GNU General Public License. Please log your changes
 *	at the end of this comment.
 *
 * License:	This program is free software; you can redistribute it and/or modify
 *			it under the terms of the GNU General Public License as published by
 *			the Free Software Foundation; either version 2 of the License, or
 *			(at your option) any later version.
 *
 *			This program is distributed in the hope that it will be useful,
 *			but WITHOUT ANY WARRANTY; without even the implied warranty of
 *			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *			GNU General Public License for more details.
 *
 *			You should have received a copy of the GNU General Public License
 *			along with this program; if not, write to the Free Software
 *			Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *

DATE:		BY:					DESCRIPTION:
==========	==================	================================================

*/

#include "stdafx.h"

#ifndef StrIsIntlEqual
#ifdef UNICODE
#define StrIsIntlEqual          StrIsIntlEqualW
#else
#define StrIsIntlEqual          StrIsIntlEqualA
#endif
#endif

#include "tools.h"

/**
 * @brief Construct and throw a Complain object containing an error message.
 */
Complain::Complain(LPCTSTR format, ...)
{
	wvsprintf(msg, format, (va_list)(&format + 1));
	throw this;
}

/**
 * @brief Report error to user. To be called from within catch block.
 */
int Complain::Alert(HWND hwndParent, UINT flags)
{
	return MessageBox(hwndParent, msg, 0, flags);
}

/**
 * @brief Complain that some DLL failed to CreateObject() something.
 */
void ComplainCreateObject(HMODULE handle, LPCTSTR name)
{
	TCHAR module[MAX_PATH];
	::GetModuleFileName(handle, module, sizeof module);
	Complain(_T("%.300s Failed to create %.300s"), module, name);
} 

/**
 * @brief Complain that something could not be found.
 */
void ComplainNotFound(LPCTSTR name)
{
	Complain(_T("Not found: %.300s"), name);
}

/**
 * @brief Complain that something could not be opened.
 */
void ComplainCantOpen(LPCTSTR name)
{
	Complain(_T("Can't open: %.300s"), name);
}

/**
 * @brief Release interface until ref count reaches 0.
 *
 * Very bad practice in general, but helps avoiding resource leaks
 * due to inaccurate ref counting.
 */
void NTAPI Release(IUnknown *punk)
{
	while (punk)
	{	
		ULONG Release = punk->Release();
		if (Release == 0)
		{
			punk = 0;
		}
	}
}

/**
 * @brief Find a keyword within an EnumList, and return its numeric value.
 */
EnumList::Find(LPCTSTR r, BOOL fCaseSens) const
{
	static const TCHAR trim[] = _T(".;:() ");
	int cch = StrCSpn(r += StrSpn(r, trim), trim);
	int Find = 0;
	LPCTSTR q = buffer;
	while (LPCTSTR p = StrChr(q, '('))
	{
		q = StrChr(++p, ')');
		if (StrIsIntlEqual(fCaseSens, p, r, cch) && q - p == cch)
			break;
		++Find;
	}
	return Find;
}
