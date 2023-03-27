/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  EditorFilepathBar.cpp
 *
 * @brief Implementation file for CEditorFilepathBar class
 */

#include "stdafx.h"
#include "EditorFilepathBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CEditorFilePathBar, CDialogBar)
	ON_NOTIFY_EX (TTN_NEEDTEXT, 0, OnToolTipNotify)
	ON_CONTROL_RANGE (EN_SETFOCUS, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnSetFocusEdit)
	ON_CONTROL_RANGE (EN_USER_CAPTION_CHANGED, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnChangeEdit)
	ON_CONTROL_RANGE (EN_USER_FILE_SELECTED, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnSelectEdit)
END_MESSAGE_MAP()


/**
 * @brief Constructor.
 */
CEditorFilePathBar::CEditorFilePathBar()
: m_nPanes(2)
{
}

/**
 * @brief Destructor.
 */
CEditorFilePathBar::~CEditorFilePathBar()
{
}

/**
 * @brief Create the window.
 * This function subclasses the edit controls.
 * @param [in] pParentWnd Parent window for edit controls.
 * @return TRUE if succeeded, FALSE otherwise.
 */
BOOL CEditorFilePathBar::Create(CWnd* pParentWnd)
{
	if (! __super::Create(pParentWnd, CEditorFilePathBar::IDD, 
			CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, CEditorFilePathBar::IDD))
		return FALSE;

	NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
	if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0))
		m_font.CreateFontIndirect(&ncm.lfStatusFont);

	// subclass the two custom edit boxes
	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	int cx = -pointToPixel(ncm.lfStatusFont.lfHeight);
	int m = pointToPixel(3);
	for (int pane = 0; pane < static_cast<int>(std::size(m_Edit)); pane++)
	{
		m_Edit[pane].SubClassEdit(IDC_STATIC_TITLE_PANE0 + pane, this);
		m_Edit[pane].SetFont(&m_font);
		m_Edit[pane].SetMargins(m, m + cx);
	}
	return TRUE;
};

CSize CEditorFilePathBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_font);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	int cy = pointToPixel(4);
	return CSize(SHRT_MAX, 1 + tm.tmHeight + cy);
}

/** 
 * @brief Resize both controls to an equal size.
 */
void CEditorFilePathBar::Resize()
{
	if (m_hWnd == nullptr)
		return;

	WINDOWPLACEMENT infoBar = {};
	GetWindowPlacement(&infoBar);

	int widths[3] = {};
	for (int pane = 0; pane < m_nPanes; pane++)
		widths[pane] = (infoBar.rcNormalPosition.right / m_nPanes) - ((pane == 0) ? 7 : 5);
	Resize(widths);
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
	if (m_hWnd == nullptr)
		return;

	// resize left filename
	CRect rc;
	int x = 0;
	GetClientRect(&rc);
	for (int pane = 0; pane < m_nPanes; pane++)
	{
		CRect rcOld;
		m_Edit[pane].GetClientRect(&rcOld);
		rc.left = x;
		rc.right = x + widths[pane] + (pane == 0 ? 5 : 7);
		x = rc.right;
		if (rcOld.Width() != rc.Width())
		{
			m_Edit[pane].MoveWindow(&rc);
			m_Edit[pane].RefreshDisplayText();
		}
	}
}

/**
 * @brief Called when tooltip is about to be shown.
 * In this function we set the tooltip text shown.
 */
BOOL CEditorFilePathBar::OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult)
{
	if (m_hWnd == nullptr)
		return FALSE;

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the CEdit 
		int nID = ::GetDlgCtrlID((HWND)pTTTStruct->idFrom);
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
			CFilepathEdit * pItem = static_cast<CFilepathEdit*>(GetDlgItem(nID));
			pTTT->lpszText = const_cast<tchar_t *>(pItem->GetUpdatedTipText(&tempDC, maxWidth).c_str());

			// set old font back
			if (hOldFont != nullptr)
				::SelectObject(tempDC.GetSafeHdc(),hOldFont);

			// we must set TTM_SETMAXTIPWIDTH to use \n in tooltips
			// just to do the first time, but how to access the tooltip during init ?
			::SendMessage(pTTTStruct->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 5000);

			return TRUE;
		}
	}
	return FALSE;
}

void CEditorFilePathBar::OnSetFocusEdit(UINT id)
{
	if (m_setFocusCallbackfunc)
		m_setFocusCallbackfunc(id - IDC_STATIC_TITLE_PANE0);
}

void CEditorFilePathBar::OnChangeEdit(UINT id)
{
	const int pane = id - IDC_STATIC_TITLE_PANE0;
	if (m_captionChangedCallbackfunc)
	{
		CString text;
		m_Edit[pane].GetWindowText(text);
		m_captionChangedCallbackfunc(pane, (const tchar_t*)text);
	}
}

void CEditorFilePathBar::OnSelectEdit(UINT id)
{
	const int pane = id - IDC_STATIC_TITLE_PANE0;
	(m_fileSelectedCallbackfunc ? m_fileSelectedCallbackfunc : m_folderSelectedCallbackfunc)
		(pane, m_Edit[pane].GetSelectedPath());
}

/** 
 * @brief Get the path for one side
 *
 * @param [in] pane Index (0-based) of pane to update.
 */
String CEditorFilePathBar::GetText(int pane) const
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return _T("");

	CString str;
	m_Edit[pane].GetWindowText(str);
	return String(str);
}

/** 
 * @brief Set the path for one side
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] lpszString New text for pane.
 */
void CEditorFilePathBar::SetText(int pane, const String& sString)
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return;

	m_Edit[pane].SetOriginalText(sString);
}

/** 
 * @brief Set the active status for one status (change the appearance)
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] bActive If `true` activates pane, `false` deactivates.
 */
void CEditorFilePathBar::SetActive(int pane, bool bActive)
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return;

	m_Edit[pane].SetActive(bActive);
}
