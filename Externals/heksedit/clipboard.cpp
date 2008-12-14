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
 * @file  clipboard.cpp
 *
 * @brief Clipboard helper functions implementation.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: clipboard.cpp 18 2008-08-18 14:46:22Z kimmov $

#include "precomp.h"
#include "clipboard.h"

//============================================================================================

void TextToClipboard(HWND hwnd, LPTSTR text, int len)
{
	// Changed for pabs's patch to compare files command.
	if (HGLOBAL hGlobal = GlobalAlloc(GHND, len * sizeof(TCHAR)))
	{
		SetCursor (LoadCursor (NULL, IDC_WAIT)); //tell user to wait
		if (LPTSTR pd = (LPTSTR)GlobalLock(hGlobal)) // get pointer to clip data
		{
			//succesfuly got pointer
			lstrcpyn(pd, text, len);//copy Text into global mem
			GlobalUnlock(hGlobal);//unlock global mem
			if (OpenClipboard(hwnd))
			{
				// open clip
				EmptyClipboard(); //empty clip
				SetClipboardData(CF_PLAINTEXT, hGlobal);//copy to clip
				CloseClipboard(); //close clip
			}
			else
			{//failed to open clip
				MessageBox(hwnd, _T("Cannot get access to clipboard."), _T("Copy"), MB_ICONERROR);
			}
		}
		else
		{//failed to get pointer to global mem
			GlobalFree(hGlobal);
			MessageBox(hwnd, _T("Cannot lock clipboard."), _T("Copy"), MB_ICONERROR);
		}
		SetCursor (LoadCursor (NULL, IDC_ARROW));//user can stop waiting
	}
	else
	{// failed to allocate global mem
		MessageBox(hwnd, _T("Not enough memory for copying."), _T("Copy"), MB_ICONERROR);
	}
}

//Pabs changed - mutated TextToClipboard into two functions
void TextToClipboard(HWND hwnd, LPTSTR text)
{
	int len = 1 + lstrlen(text);
	TextToClipboard(hwnd, text, len);
}

void MessageCopyBox(HWND hwnd, LPTSTR text, LPCTSTR caption, UINT type)
{
	int len = lstrlen(text);//get the # bytes needed to store the string (not counting '\0')
	//& get where we have to put a '\0' character later
	// RK: Added "?" to end of string.
	lstrcat(text, _T("\nDo you want the above output to be copied to the clipboard?\n"));
	if (IDYES == MessageBox(hwnd, text, caption, MB_YESNO | type))
	{
		//user wants to copy output
		text[len] = '\0';//Remove the line added above
		//Pabs removed & replaced with TextToClipboard
		TextToClipboard(hwnd, text, len + 1);
	}
//user doesn't want to copy output
}
