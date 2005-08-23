/* tools.cpp: Supplementary classes and functions for Merge7z
 * Copyright (c) 2003 Jochen Tucht
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

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003/12/09	Jochen Tucht		Created
2005/02/26	Jochen Tucht		Changed as explained in revision.txt
2005/08/20	Jochen Tucht		Remove unused EnumList
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

Complain::Complain(DWORD dwError, LPCTSTR pszContext, HMODULE hContext)
{
	LPTSTR pszMessage = msg;
	if (pszContext)
	{
		pszMessage += wsprintf(pszMessage, _T("%.500s"), pszContext);
		if (hContext)
		{
			*pszMessage++ = '@';
			int cch = ::GetModuleFileName(hContext, pszMessage, 500);
			if (cch == 0)
			{
				cch = wsprintf(pszMessage, _T("%08lX"), hContext);
			}
			pszMessage += cch;
		}
		*pszMessage++ = ':';
		*pszMessage++ = '\n';
	}
	FormatMessage
	(
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		pszMessage, 500, NULL
	);
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
	TCHAR szContext[800];
	LPTSTR pszContext = szContext + wsprintf(szContext, _T("%.100s@"), name);
	::GetModuleFileName(handle, pszContext, 500);
	Complain(RPC_S_INTERFACE_NOT_FOUND, szContext);
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

