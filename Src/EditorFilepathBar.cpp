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
 * @file  DiffList.cpp
 *
 * @brief Implementation file for CEditorFilepathBar class
 */
// ID line follows -- this is updated by SVN
// $Id: EditorFilepathBar.cpp 5401 2008-05-30 12:09:27Z kimmov $

#include "stdafx.h"
#include "Merge.h"
#include "EditorFilepathBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CEditorFilePathBar, CDialogBar)
	ON_NOTIFY_EX (TTN_NEEDTEXT, 0, OnToolTipNotify)
END_MESSAGE_MAP()


/**
 * @brief Constructor.
 */
CEditorFilePathBar::CEditorFilePathBar()
: m_pFont(NULL)
{
	m_nPanes = 2;
}

/**
 * @brief Destructor.
 */
CEditorFilePathBar::~CEditorFilePathBar()
{
	delete m_pFont;
}

/**
 * @brief Create the window.
 * This function subclasses the edit controls.
 * @param [in] pParentWnd Parent window for edit controls.
 * @return TRUE if succeeded, FALSE otherwise.
 */
BOOL CEditorFilePathBar::Create(CWnd* pParentWnd)
{
	if (! CDialogBar::Create(pParentWnd, CEditorFilePathBar::IDD, 
			CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, CEditorFilePathBar::IDD))
		return FALSE;

	// subclass the two custom edit boxes
	for (int pane = 0; pane < countof(m_Edit); pane++)
		m_Edit[pane].SubClassEdit(IDC_STATIC_TITLE_PANE0 + pane, this);

	return TRUE;
};

void CEditorFilePathBar::SetPaneCount(int nPanes)
{
	m_nPanes = nPanes;
}

/**
 * @brief Set look of headerbars similar to other window.
 *
 * @param [in] pWnd Pointer to window we want to imitate
 * @return TRUE if parent must recompute layout
 */
BOOL CEditorFilePathBar::LookLikeThisWnd(const CWnd * pWnd)
{
	// Update font. Note that we must delete previous font
	// before creating a new one.
	CFont * pFont = pWnd->GetFont();
	if (pFont)
	{
		if (m_pFont != NULL)
			delete m_pFont;

		m_pFont = new CFont();

		if (m_pFont != NULL)
		{
			LOGFONT lfFont = {0};
			if (pFont->GetLogFont(&lfFont))
			{
				m_pFont->CreateFontIndirect(&lfFont);
				for (int pane = 0; pane < m_nPanes; pane++)
					m_Edit[pane].SetFont(m_pFont);
			}
		}
	}

	// Set same dimensions (than window we imitate)
	CRect rectNew;
	pWnd->GetWindowRect(rectNew);
	CRect rectCurrent;
	GetWindowRect(rectCurrent);
	if (rectNew != rectCurrent)
	{
		SetWindowPos(NULL,0,0,rectNew.Width(), rectNew.Height(),
			SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);     
		return TRUE;
	}
	return FALSE;
}

/** 
 * @brief Resize both controls to an equal size.
 */
void CEditorFilePathBar::Resize()
{
	if (m_hWnd == NULL)
		return;

	WINDOWPLACEMENT infoBar;
	GetWindowPlacement(&infoBar);

	CRect rc;
	GetClientRect(&rc);
	for (int pane = 0; pane < m_nPanes; pane++)
	{
		int width = infoBar.rcNormalPosition.right / m_nPanes;
		rc.left = pane * width;
		rc.right = rc.left + width;
		m_Edit[pane].MoveWindow(&rc);
		m_Edit[pane].RefreshDisplayText();
	}
}
/** 
 * @brief Set widths.
 * This function resizes both controls to given size. The width is usually
 * same as the splitter view width.
 * @param [in] leftWidth Left-side control width.
 * @param [in] rightWidth Right-side control width.
 */
void CEditorFilePathBar::Resize(int widths[])
{
	if (m_hWnd == NULL)
		return;

	// resize left filename
	CRect rc;
	int x = 0;
	GetClientRect(&rc);
	for (int pane = 0; pane < m_nPanes; pane++)
	{
		rc.left = x;
		rc.right = x + widths[pane] + 4;
		x += widths[pane] + 7;
		m_Edit[pane].MoveWindow(&rc);
		m_Edit[pane].RefreshDisplayText();
	}
}

/**
 * @brief Called when tooltip is about to be shown.
 * In this function we set the tooltip text shown.
 */
BOOL CEditorFilePathBar::OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult)
{
	if (m_hWnd == NULL)
		return FALSE;

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID =pTTTStruct->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the CEdit 
		nID = ::GetDlgCtrlID((HWND)nID);
		if(nID == IDC_STATIC_TITLE_PANE0 || nID == IDC_STATIC_TITLE_PANE1 || nID == IDC_STATIC_TITLE_PANE2)
		{
			// compute max width : 97% of application width or 80% or full screen width
			CRect rect;
			GetWindowRect(rect);
			int maxWidth = (int)(rect.Width() * .97);
			CRect rectScreen; 
			SystemParametersInfo(SPI_GETWORKAREA, 0, rectScreen, 0);
			if (rectScreen.Width() * .8 > maxWidth)
				maxWidth = (int)(rectScreen.Width() * .8);

			// use the tooltip font
			HANDLE hFont = (HANDLE) ::SendMessage(pTTTStruct->hwndFrom, WM_GETFONT, 0, 0);
			CClientDC tempDC(this);
			HANDLE hOldFont = ::SelectObject(tempDC.GetSafeHdc(),hFont);

			// fill in the returned structure
			CFilepathEdit * pItem = (CFilepathEdit*) GetDlgItem(nID);
			pTTT->lpszText = (TCHAR*) pItem->GetUpdatedTipText(&tempDC, maxWidth);

			// set old font back
			if (hOldFont)
				::SelectObject(tempDC.GetSafeHdc(),hOldFont);

			// we must set TTM_SETMAXTIPWIDTH to use \n in tooltips
			// just to do the first time, but how to access the tooltip during init ?
			::SendMessage(pTTTStruct->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 5000);

			return(TRUE);
		}
	}
	return(FALSE);
}

/** 
 * @brief Set the path for one side
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] lpszString New text for pane.
 */
void CEditorFilePathBar::SetText(int pane, LPCTSTR lpszString)
{
	ASSERT (pane >= 0 && pane < countof(m_Edit));

	// Check for NULL since window may be closing..
	if (m_hWnd == NULL)
		return;

	m_Edit[pane].SetOriginalText(lpszString);
}

/** 
 * @brief Set the active status for one status (change the appearance)
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] bActive If TRUE activates pane, FALSE deactivates.
 */
void CEditorFilePathBar::SetActive(int pane, BOOL bActive)
{
	ASSERT (pane >= 0 && pane < countof(m_Edit));

	// Check for NULL since window may be closing..
	if (m_hWnd == NULL)
		return;

	m_Edit[pane].SetActive(bActive);
}
