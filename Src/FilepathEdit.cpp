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
/** 
 * @file  FilePathEdit.cpp
 *
 * @brief Implementation of the CFilepathEdit class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "BCMenu.h"
#include "FilepathEdit.h"
#include "Shlwapi.h"
#include "dllVer.h"		

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilepathEdit message handlers
BEGIN_MESSAGE_MAP(CFilepathEdit, CEdit)
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


/** 
 * @brief Format path for display in header control. 
 *
 * Formats path so it fits to given lenght, tries to end lines after
 * slash chars.
 *
 * @param [in] pDC Pointer to draw context.
 * @param [in] maxWidth Maximum width of string.
 * @param [in,out] sFilepath:
 * - in: string to format
 * - out: formatted string
 * @return Number of lines path is splitted to.
 */
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
CFilepathEdit::CFilepathEdit()
 : m_crBackGnd(RGB(255, 255, 255))
 , m_crText(RGB(0,0,0))
{
}

BOOL CFilepathEdit::SubClassEdit(UINT nID, CWnd* pParent)
{
	m_bActive = FALSE;
	return SubclassDlgItem(nID, pParent);
};

/////////////////////////////////////////////////////////////////////////////
// CFilepathEdit

/** Return the entire path originally given to this edit control */
void CFilepathEdit::GetWholeText(CString& rString) const
{		
	rString = m_sWholeText;
}

/**
 * @brief Store path given, and also compute display version (may be shortened)
 *
 * The original path is saved as m_sWholeText.
 * The display version is passed to underlying edit control (via SetWindowText)
 */
void CFilepathEdit::SetWholeText(LPCTSTR lpszString)
{
	if (_tcscmp(m_sWholeText, lpszString) == 0)
		return;

	if (lpszString != 0)
		m_sWholeText = lpszString;

	RefreshDisplayText();
}

/**
 * @brief Recompute display text from m_sWholeText & update window text
 *
 * This method takes the m_sWholeText string and computes a short version
 * and uses the short version to set the window text (which will be used
 * by the underlying edit control to actually paint).
 */
void CFilepathEdit::RefreshDisplayText()
{
	CString line = m_sWholeText;

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
	PathCompactPath(lDC.GetSafeHdc(), line.GetBuffer(line.GetLength()+3), rect.Width());
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

/**
 * @brief Updates and returns the tooltip for this edit box
 *
 * @note This uses a member variable (m_sToolTipString) in 
 * order to be able to return an LPCTSTR safely.
 */
LPCTSTR CFilepathEdit::GetUpdatedTipText(CDC * pDC, int maxWidth)
{
	GetWholeText(m_sToolTipString);
	FormatFilePathForDisplayWidth(pDC, maxWidth, m_sToolTipString);
	return (LPCTSTR)m_sToolTipString;
}

/**
 * @brief retrieve text from the wholeText
 *
 * @note The standard Copy function works with the (compacted) windowText 
 */
void CFilepathEdit::CustomCopy(int iBegin, int iEnd /*=-1*/)
{
	if (iEnd == -1)
		iEnd = m_sWholeText.GetLength();

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
	// Copy selected data from m_sWholeText into the alloc'd data area
	_tcscpy (pszData, (LPTSTR) m_sWholeText.Mid(iBegin, iEnd-iBegin).GetBuffer(0));
	GlobalUnlock (hData);
	UINT fmt = GetClipTcharTextFormat();      // CF_TEXT or CF_UNICODETEXT
	// Using alloc'd data, set the clipboard
	SetClipboardData (fmt, hData);

	// release the clipboard
	CloseClipboard ();
}

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

		BCMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_EDITOR_HEADERBAR));

		BCMenu* pPopup = (BCMenu *) menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		if (m_sWholeText.Right(1) == '\\')
			// no filename, we have to disable the unwanted menu entry
			pPopup->EnableMenuItem(ID_EDITOR_COPY_FILENAME, MF_GRAYED);

		// invoke context menu
		// we don't want to use the main application handlers, so we use flags TPM_NONOTIFY | TPM_RETURNCMD
		// and handle the command after TrackPopupMenu
		int command = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY  | TPM_RETURNCMD, point.x, point.y, AfxGetMainWnd());
		if (command != ID_EDITOR_COPY_FILENAME && command != ID_EDITOR_COPY_PATH)
			return;

		// compute the beginning of the text to copy (in wholeText)
		int iBegin;
		switch (command)
		{
		case ID_EDITOR_COPY_FILENAME:
			{
			int lastSlash = m_sWholeText.ReverseFind('\\');
			if (lastSlash != -1)
				iBegin = lastSlash+1;
			else
				iBegin = 0;
			}
			break;
		case ID_EDITOR_COPY_PATH:
			// pass the heading "*" for modified files
			if (m_sWholeText.GetAt(0) == '*')
				iBegin = 2;
			else
				iBegin = 0;
			break;
		}
		
		CustomCopy(iBegin);
	}
}

void CFilepathEdit::SetActive(BOOL bActive)
{
	m_bActive = bActive;

	if (m_hWnd == NULL)
		return;

	CRect rcWnd;
	GetWindowRect(&rcWnd);

	if (bActive)
	{
		SetTextColor(::GetSysColor(COLOR_CAPTIONTEXT));
		SetBackColor(::GetSysColor(COLOR_ACTIVECAPTION));
	}
	else
	{
		SetTextColor(::GetSysColor(COLOR_INACTIVECAPTIONTEXT));
		SetBackColor(::GetSysColor(COLOR_INACTIVECAPTION));
	}
}

HBRUSH CFilepathEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// Return a non-NULL brush if the parent's 
	//handler should not be called

	//set text color
	pDC->SetTextColor(m_crText);

	//set the text's background color
	pDC->SetBkColor(m_crBackGnd);

	//return the brush used for background this sets control background
	return m_brBackGnd;
}

void CFilepathEdit::SetBackColor(COLORREF rgb)
{
	//set background color ref (used for text's background)
	m_crBackGnd = rgb;
	
	//free brush
	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
	//set brush to new color
	m_brBackGnd.CreateSolidBrush(rgb);
	
	//redraw
	Invalidate(TRUE);
}
void CFilepathEdit::SetTextColor(COLORREF rgb)
{
	//set text color ref
	m_crText = rgb;

	//redraw
	Invalidate(TRUE);
}

