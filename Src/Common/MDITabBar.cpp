/**
 * @file  MDITabBar.cpp
 *
 * @brief Implementation of the MDITabBar class
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "MDITabBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDITabBar

IMPLEMENT_DYNAMIC(CMDITabBar, CControlBar)

BEGIN_MESSAGE_MAP(CMDITabBar, CControlBar)
	//{{AFX_MSG_MAP(CMDITabBar)
	ON_WM_MBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	ON_WM_DRAWITEM_REFLECT()
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

	if (!CWnd::Create(WC_TABCONTROL, NULL, WS_CHILD | WS_VISIBLE | TCS_OWNERDRAWFIXED, CRect(0, 0, 0, 0), pMainFrame, AFX_IDW_CONTROLBAR_FIRST+30))
		return FALSE;

	TabCtrl_SetPadding(m_hWnd, 16, 4);
	SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

	return TRUE;
}


/** 
 * @brief This method calculates the horizontal size of a control bar.
 */
CSize CMDITabBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (GetItemCount() == 0)
		return CSize(SHRT_MAX, 0);
	
	TEXTMETRIC tm;
	CDC *pdc = GetDC();
	pdc->GetTextMetrics(&tm);
	ReleaseDC(pdc);

	return CSize(SHRT_MAX, tm.tmHeight + 8);
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
	MENUITEMINFO mii = {0};
	mii.cbSize = sizeof(MENUITEMINFO);
	if (!pPopup->GetMenuItemInfo(ID_CLOSE_OTHER_TABS, &mii, FALSE))
	{
		pPopup->AppendMenu(MF_SEPARATOR, 0, _T(""));
		pPopup->AppendMenu(MF_STRING, ID_CLOSE_OTHER_TABS, _("Close &Other Tabs").c_str());
		pPopup->AppendMenu(MF_STRING, ID_CLOSE_RIGHT_TABS, _("Close R&ight Tabs").c_str());
		pPopup->AppendMenu(MF_STRING, ID_CLOSE_LEFT_TABS, _("Close &Left Tabs").c_str());
	}
	// invoke context menu
	int command = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y,
		this);
	switch (command)
	{
	case ID_CLOSE_OTHER_TABS:
	case ID_CLOSE_RIGHT_TABS:
	case ID_CLOSE_LEFT_TABS: {
		int curcel = GetCurSel();
		int n = GetItemCount();
		TCITEM tci;
		tci.mask = TCIF_PARAM;
		for (int i = n - 1; i >= 0; --i)
		{
			if ((command == ID_CLOSE_OTHER_TABS && i == curcel) ||
				(command == ID_CLOSE_RIGHT_TABS && i <= curcel) ||
				(command == ID_CLOSE_LEFT_TABS  && i >= curcel))
				continue;
			GetItem(i, &tci);
			CWnd* pMDIChild = FromHandle((HWND)tci.lParam);
			pMDIChild->SendMessage(WM_SYSCOMMAND, SC_CLOSE);
		}
		break;
	}
	default:
		pMDIChild->SendMessage(WM_SYSCOMMAND, command);
	}
}

/**
 * @brief synchronize the tabs with all mdi client windows.
 */
void CMDITabBar::UpdateTabs()
{
	Invalidate();

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

	int nMaxTitleLength = static_cast<int>(MDITABBAR_MAXTITLELENGTH - (MDIFrameList.GetCount() - 1) * 6);
	if (nMaxTitleLength < MDITABBAR_MINTITLELENGTH)
		nMaxTitleLength = MDITABBAR_MINTITLELENGTH;

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

		if (strTitle.GetLength() > nMaxTitleLength)
			strTitle = strTitle.Left(nMaxTitleLength - 3) + _T("...");

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
			tci.cchTextMax = countof(szText);
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

/**
 * @brief Called when middle mouse button is pressed.
 * This function closes the tab when the middle mouse button is pressed.
 */
void CMDITabBar::OnMButtonDown(UINT nFlags, CPoint point)
{
	TCHITTESTINFO hit;
	hit.pt = point;
	int index = HitTest(&hit);
	if (index < 0)
		return;

	TCITEM tci;
	tci.mask = TCIF_PARAM;
	GetItem(index, &tci);
	CWnd* pMDIChild = FromHandle((HWND)tci.lParam);
	pMDIChild->SendMessage(WM_SYSCOMMAND, SC_CLOSE);
}

void CMDITabBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
		TCHAR            szBuf[256];
		TCITEM           item;
		LPDRAWITEMSTRUCT lpDraw = (LPDRAWITEMSTRUCT)lpDrawItemStruct;

		item.mask       = TCIF_TEXT | TCIF_PARAM;
		item.pszText    = szBuf;
		item.cchTextMax = sizeof(szBuf) / sizeof(TCHAR);
		TabCtrl_GetItem(this->m_hWnd, lpDraw->itemID, &item);

		RECT rc = lpDraw->rcItem;
		if (lpDraw->itemState & ODS_SELECTED)
		{
			rc.left += 9;
			rc.top += 2;
			FillRect(lpDraw->hDC, &lpDraw->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
		}
		else
		{
			rc.left += 5;
			rc.top += 3;
		}
		rc.left += 16;
		SetTextColor(lpDraw->hDC, RGB(0, 0, 0));
		SetBkMode(lpDraw->hDC, TRANSPARENT);
		HICON hIcon = CWnd::FromHandle((HWND)item.lParam)->GetIcon(TRUE);
		if (!hIcon)
			hIcon = (HICON)GetClassLongPtr((HWND)item.lParam, GCLP_HICONSM);
		if (hIcon)
			DrawIconEx(lpDraw->hDC, rc.left - 16 - 2, 5, hIcon, 16, 16, 0, NULL, DI_NORMAL);
		DrawText(lpDraw->hDC, szBuf, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}
