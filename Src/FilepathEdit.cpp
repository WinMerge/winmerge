/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// FilepathEdit.cpp : implementation of the CFilepathEdit class
//

#include "stdafx.h"
#include "Merge.h"

#include "FilepathEdit.h"

// for PathCompactPath
//#include "Shlwapi.h"		// dynamically linked
#include "dllVer.h"		


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

// safe PathCompactPath call
// as it is not supported for shlwapi.dll before 4.71

typedef BOOL (STDAPICALLTYPE *PathCompactPathFnc)(
	HDC hDC,
	LPTSTR lpszPath,
	UINT dx);

#ifdef UNICODE
// Windows doesn't seem to have Unicode version of GetProcAddress, so both strings here are 8-bit :(
#define PathCompactPathName "PathCompactPathW"
#else
#define PathCompactPathName "PathCompactPathA"
#endif

static BOOL PathCompactPathDynamic(HDC hdc, LPTSTR lpszPath, UINT dx)
{
	if (GetDllVersion(_T("shlwapi.dll")) < PACKVERSION(4,71))
	{
		// not supported before 4.71
		return FALSE;
	}

	Library lib;
	if (!lib.Load(_T("shlwapi.dll")))
	{
		// Shouldn't happen; GetDllVersion loaded this successfully, oh well
		return FALSE;
	}
	FARPROC proc = GetProcAddress(lib.Inst(), PathCompactPathName);
	if (!proc)
	{
		// Shouldn't happen, 4.71 should have this, oh well
		return FALSE;
	}
	PathCompactPathFnc pathproc = (PathCompactPathFnc)proc;
	return (*pathproc)(hdc, lpszPath, dx);
}

// format a path in a pDC box of width maxWidth
// try to cut lines only at slash characters
int FormatFilePathForDisplayWidth(CDC * pDC, int maxWidth, CString & sFilepath)
{
	int iBegin = 0;
	int nLines = 1;
	
	while (1)
	{
		CString line;

		// find the next truncation point
		int iEndMin = 0;
		int iEndMax = sFilepath.GetLength()-iBegin + 1;
		while(1)
		{
			int iEnd = (iEndMin + iEndMax) / 2;
			if (iEnd == iEndMin)
				break;
			line = sFilepath.Mid(iBegin, iEnd);
			int width = (pDC->GetTextExtent(line)).cx;
			if (width > maxWidth)
				iEndMax = iEnd;
			else
				iEndMin = iEnd;
		};
		ASSERT(iEndMax == iEndMin+1);

		// here iEndMin is the last character displayed in maxWidth

		// exit the loop if we can display the remaining characters with no truncation
		if (iBegin+iEndMin == sFilepath.GetLength())
			break;

		// truncate the text to the previous "\" if possible
		line = sFilepath.Mid(iBegin, iEndMin);
		int lastSlash = line.ReverseFind('\\');
		if (lastSlash >= 0)
			iEndMin = lastSlash+1;

		sFilepath.Insert(iBegin+iEndMin, _T("\n"));
		iBegin += iEndMin+2;
		nLines ++;
	}

	return nLines;
}

/////////////////////////////////////////////////////////////////////////////
// CFilepathEdit construction/destruction

BOOL CFilepathEdit::SubClassEdit(UINT nID, CWnd* pParent)
{
	return SubclassDlgItem(nID, pParent);
};

/////////////////////////////////////////////////////////////////////////////
// CFilepathEdit

void CFilepathEdit::GetWholeText(CString& rString) const
{		
	rString = wholeText;
}

// set whole text
// and set the display text (truncate it if necessary)
void CFilepathEdit::SetWholeText(LPCTSTR lpszString)
{
	if (_tcscmp(wholeText, lpszString) == 0)
		return;

	if (lpszString != 0)
		wholeText = lpszString;

	RefreshDisplayText();
}

// the display test is the window text for CWnd
void CFilepathEdit::RefreshDisplayText()
{
	CString line = wholeText;

	// we want to keep the first and the last path component, and in between,
	// as much characters as possible from the right
	// PathCompactPath keeps, in between, as much characters as possible from the left
	// so we reverse everything between the first and the last component before calling PathCompactPath
	int iBeginLast, iEndIntro;
	iBeginLast = line.ReverseFind('\\');
	iEndIntro = line.Find('\\');
	if (iBeginLast >= 0 && iEndIntro != iBeginLast)
	{
		CString textToReverse = line.Mid(iEndIntro+1, iBeginLast-(iEndIntro+1));
		textToReverse.MakeReverse();
		line = line.Left(iEndIntro+1) + textToReverse + line.Mid(iBeginLast);
	}

	// get a device context object
	CClientDC lDC(this);
	// and use the correct font
	CFont * pFontOld = lDC.SelectObject(GetFont());	

	// compact the path
	CRect rect;
	GetRect(rect);
	// take GetBuffer (lenght +3) to count for ellipsis
	PathCompactPathDynamic(lDC.GetSafeHdc(), line.GetBuffer(line.GetLength()+3), rect.Width());
	line.ReleaseBuffer();
	
	// set old font back
	lDC.SelectObject(pFontOld);

	// we reverse back everything between the first and the last component
	// it works OK as "..." reversed = "..." again
	iBeginLast = line.ReverseFind('\\');
	iEndIntro = line.Find('\\');
	if (iBeginLast >= 0 && iEndIntro != iBeginLast)
	{
		CString textToReverse = line.Mid(iEndIntro+1, iBeginLast-(iEndIntro+1));
		textToReverse.MakeReverse();
		line = line.Left(iEndIntro+1) + textToReverse + line.Mid(iBeginLast);
	}

	SetWindowText(line);
}

// updates and returns the tooltip for this edit box
LPCTSTR CFilepathEdit::GetUpdatedTipText(CDC * pDC, int maxWidth)
{
	GetWholeText(toolTipString);

	if (GetDllVersion(_T("shlwapi.dll")) < PACKVERSION(4,70))
		// \n in tooltip text not supported before 4.70
		;
	else
		FormatFilePathForDisplayWidth(pDC, maxWidth, toolTipString);
		
	// add the help text
// toolTipString += "\n\nRight click on the path to copy";

	return (LPCTSTR) toolTipString;
}

/**
 * @brief retrieve text from the wholeText
 *
 * @note The standard Copy function works with the (compacted) windowText 
 */
void CFilepathEdit::CustomCopy(int iBegin, int iEnd /*=-1*/)
{
	if (iEnd == -1)
		iEnd = wholeText.GetLength();

	// get the clipboard
	if (! OpenClipboard())
		return;
	if (! EmptyClipboard())
		return;		
		
	// insert text into clipboard
	HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, (iEnd-iBegin + 1)*sizeof(TCHAR));
	if (hData == NULL)
		return;
	LPTSTR pszData = (LPTSTR)::GlobalLock (hData);
	_tcscpy (pszData, (LPTSTR) wholeText.Mid(iBegin, iEnd-iBegin).GetBuffer(0));
	GlobalUnlock (hData);
	UINT fmt = GetClipTcharTextFormat();      // CF_TEXT or CF_UNICODETEXT
	SetClipboardData (fmt, hData);

	// release the clipboard
	CloseClipboard ();
}


/////////////////////////////////////////////////////////////////////////////
// CFilepathEdit message handlers
BEGIN_MESSAGE_MAP(CFilepathEdit, CEdit)
  ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


void CFilepathEdit::OnContextMenu(CWnd*, CPoint point)
{
	{
		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_EDITOR_HEADERBAR));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		// invoke context menu
		// we don't want to use the main application handlers, so we use flags TPM_NONOTIFY | TPM_RETURNCMD
		// we have to disable unwanted menu entries (everything is enabled so nothing to do)
		// and handle the command after TrackPopupMenu
		int command = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY  | TPM_RETURNCMD, point.x, point.y, this);
		if (command != ID_EDITOR_COPY_FILENAME && command != ID_EDITOR_COPY_PATH)
			return;

		// compute the beginning of the text to copy (in wholeText)
		int iBegin;
		switch (command)
		{
		case ID_EDITOR_COPY_FILENAME:
			{
			int lastSlash = wholeText.ReverseFind('\\');
			if (lastSlash != -1)
				iBegin = lastSlash+1;
			else
				iBegin = 0;
			}
			break;
		case ID_EDITOR_COPY_PATH:
			// pass the heading "*" for modified files
			if (wholeText.GetAt(0) == '*')
				iBegin = 2;
			else
				iBegin = 0;
			break;
		}
		
		CustomCopy(iBegin);
	}
}

