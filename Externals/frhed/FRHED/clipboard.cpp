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
 * @file  clipboard.cpp
 *
 * @brief Clipboard helper functions implementation.
 *
 */
#include "precomp.h"
#include "clipboard.h"
#include "resource.h"
#include "StringTable.h"
#include "hexwnd.h"
#include "hexwdlg.h"

/**
 * @brief Copy text to clipboard.
 * @param [in] hwnd Parent window handle.
 * @param [in] text Text to copy.
 * @param [in] len Lenght of the text in characters.
 */
void TextToClipboard(HWindow *pwnd, TCHAR *text, SIZE_T len)
{
	const SIZE_T size = (len + 1) * sizeof(TCHAR);
	if (HGLOBAL hGlobal = GlobalAlloc(GHND, size))
	{
		WaitCursor wc;
		if (TCHAR *pd = (TCHAR *)GlobalLock(hGlobal))
		{
			// Succesfully got pointer
			_tcsncpy(pd, text, len);
			GlobalUnlock(hGlobal);
			if (pwnd->OpenClipboard())
			{
				// Open clip
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hGlobal);
				CloseClipboard();
			}
			else // Failed to open clip
			{
				MessageBox(pwnd, GetLangString(IDS_CANNOT_ACCESS_CLIPBOARD), MB_ICONERROR);
			}
		}
		else
		{
			// Failed to get pointer to global mem
			GlobalFree(hGlobal);
			MessageBox(pwnd, GetLangString(IDS_CANNOT_LOCK_CLIPBOARD), MB_ICONERROR);
		}
	}
	else
	{
		// Failed to allocate global mem
		MessageBox(pwnd, GetLangString(IDS_CLIPBOARD_NO_MEM), MB_ICONERROR);
	}
}

/**
 * @brief Copy text to clipboard.
 * @param [in] hwnd Parent window handle.
 * @param [in] text Text to copy.
 */
void TextToClipboard(HWindow *pwnd, TCHAR *text)
{
	SIZE_T len = _tcslen(text);
	TextToClipboard(pwnd, text, len);
}

void MessageCopyBox(HWindow *pwnd, LPTSTR text, UINT type)
{
	SIZE_T len = _tcslen(text);
	_tcscat(text, GetLangString(IDS_CLIPBOARD_COPY_TO));
	if (IDYES == MessageBox(pwnd, text, MB_YESNO | type))
	{
		// User wants to copy output
		text[len] = '\0'; // Remove the line added above
		TextToClipboard(pwnd, text, len);
	}
}
