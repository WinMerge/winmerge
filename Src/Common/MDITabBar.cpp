/**
 * @file  MDITabBar.cpp
 *
 * @brief Implementation of the MDITabBar class
 */
// RCS ID line follows -- this is updated by CVS
// $Id: $

#include "stdafx.h"
#include "MDITabBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MDITABBAR_MAXTITLELENGTH 64

/////////////////////////////////////////////////////////////////////////////
// CMDITabBar

IMPLEMENT_DYNAMIC(CMDITabBar, CControlBar)

BEGIN_MESSAGE_MAP(CMDITabBar, CControlBar)
	//{{AFX_MSG_MAP(CMDITabBar)
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Create tab bar.
 * @param pParentWnd [in] main frame window pointer
 */
BOOL CMDITabBar::Create(CMDIFrameWnd* pMainFrame)
{
	m_pMainFrame = pMainFrame;
	m_dwStyle = CBRS_TOP;

	if (!CWnd::Create(WC_TABCONTROL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pMainFrame, AFX_IDW_CONTROLBAR_FIRST+30))
		return FALSE;

	SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

	return TRUE;
}


/** 
 * @brief This method calculates the horizontal size of a control bar.
 */
CSize CMDITabBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (GetItemCount() == 0)
		return CSize(32767, 0);
	
	TEXTMETRIC tm;
	CDC *pdc = GetDC();
	pdc->GetTextMetrics(&tm);
	ReleaseDC(pdc);

	return CSize(32767, tm.tmHeight + 4);
}

void CMDITabBar::OnPaint() 
{
	Default();
}

/** 
 * @brief Called when tab selection is changed.
 */
BOOL CMDITabBar::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	GetItem(GetCurSel(), &tci);
	m_bInSelchange = TRUE;
	m_pMainFrame->MDIActivate(FromHandle((HWND)tci.lParam));
	m_bInSelchange = FALSE;

	return TRUE;
}

/**
 * @brief Show context menu and handle user selection.
 */
void CMDITabBar::OnContextMenu(CWnd *pWnd, CPoint point)
{
	TCHITTESTINFO hit;
	hit.pt = point;
	ScreenToClient(&hit.pt);
	int index = HitTest(&hit);
	if (index < 0) return;

	TCITEM tci;
	tci.mask = TCIF_PARAM;
	GetItem(index, &tci);
	CWnd* pMDIChild = FromHandle((HWND)tci.lParam);
	m_pMainFrame->MDIActivate(pMDIChild);
	CMenu* pPopup = pMDIChild->GetSystemMenu(FALSE);
	if (!pPopup) return;

	// invoke context menu
	int command = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y,
		AfxGetMainWnd());
	pMDIChild->SendMessage(WM_SYSCOMMAND, command);
}

/**
 * @brief synchronize the tabs with all mdi client windows.
 */
void CMDITabBar::UpdateTabs()
{
	HWND hWndMDIActive = m_pMainFrame->MDIGetActive()->GetSafeHwnd();

	CMap<HWND, HWND, int, int> MDIFrameList;
	if (hWndMDIActive) {
		for (CWnd *pFrame = m_pMainFrame->MDIGetActive()->GetParent()->GetTopWindow(); pFrame; pFrame = pFrame->GetNextWindow())
			MDIFrameList[pFrame->m_hWnd] = -1;
	}

	// Associate MDIFrameList with the index of the tab
	TC_ITEM tci;
	int item;
	for (item = GetItemCount() - 1; item >= 0 ; item--)
	{
		int dummy;
		tci.mask = TCIF_PARAM;
		GetItem(item, &tci);
		if (MDIFrameList.Lookup((HWND)tci.lParam, dummy))
		{
			MDIFrameList[(HWND)tci.lParam] = item;
			if (!m_bInSelchange && hWndMDIActive == (HWND)tci.lParam)
				SetCurSel(item);
		}
	}

	// Update or insert tabs
	for (POSITION pos = MDIFrameList.GetStartPosition(); pos; )
	{
		HWND hFrameWnd;
		int item;
		MDIFrameList.GetNextAssoc(pos, hFrameWnd, item);

		CString strTitle;
		CDocument *pDoc = ((CFrameWnd *)FromHandle(hFrameWnd))->GetActiveDocument();
		if (pDoc)
			strTitle = pDoc->GetTitle();
		else
			FromHandle(hFrameWnd)->GetWindowText(strTitle);

		if (strTitle.GetLength() > MDITABBAR_MAXTITLELENGTH)
			strTitle = strTitle.Left(MDITABBAR_MAXTITLELENGTH - 3) + _T("...");

		if (item == -1)
		{
			tci.mask = TCIF_PARAM | TCIF_TEXT;
			tci.pszText = strTitle.LockBuffer();
			tci.lParam = (LPARAM)hFrameWnd;
			InsertItem(GetItemCount(), &tci);
			if (GetItemCount() == 1)
				m_pMainFrame->RecalcLayout();
			if (!m_bInSelchange && hWndMDIActive == (HWND)tci.lParam)
				SetCurSel(item);
		}
		else
		{
			TCHAR szText[256];
			tci.pszText = szText;
			tci.cchTextMax = sizeof(szText)/sizeof(szText[0]);
			tci.mask = TCIF_TEXT;
			GetItem(item, &tci);
			if (tci.pszText && strTitle.Compare(tci.pszText) != 0)
			{
				tci.pszText = strTitle.LockBuffer();
				SetItem(item, &tci);
			}
		}
	}

	// Delete tabs
	for (item = GetItemCount() - 1; item >= 0 ; item--)
	{
		int dummy;
		tci.mask = TCIF_PARAM;
		GetItem(item, &tci);
		if (MDIFrameList.Lookup((HWND)tci.lParam, dummy) == FALSE)
		{
			DeleteItem(item);
			if (GetItemCount() == 0)
				m_pMainFrame->RecalcLayout();
		}
	}
}
