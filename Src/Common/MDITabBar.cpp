/**
 * @file  MDITabBar.cpp
 *
 * @brief Implementation of the MDITabBar class
 */

#include "StdAfx.h"
#include "MDITabBar.h"
#include "IMDITab.h"
#include "cecolor.h"
#include "RoundedRectWithShadow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr int RR_RADIUS = 3;
constexpr int RR_SHADOWWIDTH = 3;

/////////////////////////////////////////////////////////////////////////////
// CMDITabBar

IMPLEMENT_DYNAMIC(CMDITabBar, CControlBar)

BEGIN_MESSAGE_MAP(CMDITabBar, CControlBar)
	//{{AFX_MSG_MAP(CMDITabBar)
	ON_WM_MBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static int determineIconSize()
{
	return GetSystemMetrics(SM_CXSMICON);
}

/** 
 * @brief Create tab bar.
 * @param pParentWnd [in] main frame window pointer
 */
BOOL CMDITabBar::Create(CMDIFrameWnd* pMainFrame)
{
	m_pMainFrame = pMainFrame;
	m_dwStyle = CBRS_TOP;

	if (!CWnd::Create(WC_TABCONTROL, nullptr, WS_CHILD | WS_VISIBLE | TCS_OWNERDRAWFIXED, CRect(0, 0, 0, 0), pMainFrame, AFX_IDW_CONTROLBAR_FIRST+30))
		return FALSE;

	CClientDC dc(this);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int r = pointToPixel(RR_RADIUS);
	const int sw = pointToPixel(RR_SHADOWWIDTH);
	TabCtrl_SetPadding(m_hWnd, sw + r * 2 + determineIconSize() / 2, sw + r);

	NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0);
	m_font.CreateFontIndirect(&ncm.lfMenuFont);
	SetFont(&m_font);

	m_tooltips.Create(m_pMainFrame, TTS_NOPREFIX);
	m_tooltips.AddTool(this, _T(""));

	return TRUE;
}


/**
 * @brief Called before messages are translated.
 * Passes a mouse message to the ToolTip control for processing.
 * @param [in] pMsg Points to an MSG structure that contains the message to be chcecked
 */
BOOL CMDITabBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
		m_tooltips.RelayEvent(pMsg);

	// Call the parent method.
	return CControlBar::PreTranslateMessage(pMsg);
}

/** 
 * @brief This method calculates the horizontal size of a control bar.
 */
CSize CMDITabBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (GetItemCount() == 0)
		return CSize(SHRT_MAX, 0);
	
	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_font);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);

	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int r = pointToPixel(RR_RADIUS);
	const int sw = pointToPixel(RR_SHADOWWIDTH);

	return CSize(SHRT_MAX, tm.tmHeight + (sw + r) * 2);
}

void CMDITabBar::OnPaint() 
{
	CPaintDC dc(this);
	dc.SelectObject(GetFont());

	DRAWITEMSTRUCT dis;
	dis.hDC = dc.GetSafeHdc();

	CRect rcClient;
	GetClientRect(&rcClient);

	int nCurSel = GetCurSel();
	for (int i = GetItemCount() - 1; i >= 0; --i)
	{
		GetItemRect(i, &dis.rcItem);
		dis.itemID = i;
		dis.rcItem.top = rcClient.top;
		dis.rcItem.bottom = rcClient.bottom;
		dis.itemState = (i != nCurSel) ? 0 : ODS_SELECTED;
		DrawItem(&dis);
	}
}

BOOL CMDITabBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rClient;
	GetClientRect(rClient);
	pDC->FillSolidRect(rClient, GetSysColor(COLOR_3DFACE));
	return TRUE;
}

/** 
 * @brief Called when tab selection is changed.
 */
BOOL CMDITabBar::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	GetItem(GetCurSel(), &tci);
	m_bInSelchange = true;
	m_pMainFrame->MDIActivate(FromHandle((HWND)tci.lParam));
	m_bInSelchange = false;

	return TRUE;
}

/**
 * @brief Show context menu and handle user selection.
 */
void CMDITabBar::OnContextMenu(CWnd *pWnd, CPoint point)
{
	CPoint ptClient = point;
	ScreenToClient(&ptClient);
	int index = GetItemIndexFromPoint(ptClient);
	if (index < 0)
		return;

	TCITEM tci;
	tci.mask = TCIF_PARAM;
	GetItem(index, &tci);
	CWnd* pMDIChild = FromHandle((HWND)tci.lParam);
	m_pMainFrame->MDIActivate(pMDIChild);
	CMenu* pPopup = pMDIChild->GetSystemMenu(FALSE);
	if (pPopup == nullptr) return;
	MENUITEMINFO mii = { sizeof MENUITEMINFO };
	if (!pPopup->GetMenuItemInfo(ID_CLOSE_OTHER_TABS, &mii, FALSE))
	{
		pPopup->AppendMenu(MF_SEPARATOR, 0, _T(""));
		pPopup->AppendMenu(MF_STRING, ID_TABBAR_AUTO_MAXWIDTH, _T(""));
		pPopup->AppendMenu(MF_SEPARATOR, 0, _T(""));
		pPopup->AppendMenu(MF_STRING, ID_CLOSE_OTHER_TABS, _T(""));
		pPopup->AppendMenu(MF_STRING, ID_CLOSE_RIGHT_TABS, _T(""));
		pPopup->AppendMenu(MF_STRING, ID_CLOSE_LEFT_TABS, _T(""));
	}
	pPopup->ModifyMenu(ID_TABBAR_AUTO_MAXWIDTH, MF_BYCOMMAND, ID_TABBAR_AUTO_MAXWIDTH, _("Enable &Auto Max Width").c_str());
	pPopup->ModifyMenu(ID_CLOSE_OTHER_TABS, MF_BYCOMMAND, ID_CLOSE_OTHER_TABS, _("Close &Other Tabs").c_str());
	pPopup->ModifyMenu(ID_CLOSE_RIGHT_TABS, MF_BYCOMMAND, ID_CLOSE_RIGHT_TABS, _("Close R&ight Tabs").c_str());
	pPopup->ModifyMenu(ID_CLOSE_LEFT_TABS, MF_BYCOMMAND, ID_CLOSE_LEFT_TABS, _("Close &Left Tabs").c_str());

	pPopup->CheckMenuItem(ID_TABBAR_AUTO_MAXWIDTH, m_bAutoMaxWidth ? MF_CHECKED : MF_UNCHECKED);
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
		TCITEM tci1;
		tci1.mask = TCIF_PARAM;
		for (int i = n - 1; i >= 0; --i)
		{
			if ((command == ID_CLOSE_OTHER_TABS && i == curcel) ||
				(command == ID_CLOSE_RIGHT_TABS && i <= curcel) ||
				(command == ID_CLOSE_LEFT_TABS  && i >= curcel))
				continue;
			GetItem(i, &tci1);
			CWnd* pMDIChild1 = FromHandle((HWND)tci1.lParam);
			pMDIChild1->SendMessage(WM_SYSCOMMAND, SC_CLOSE);
		}
		break;
	}
	case ID_TABBAR_AUTO_MAXWIDTH:
		m_bAutoMaxWidth = !m_bAutoMaxWidth;
		UpdateTabs();
		break;
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
	if (hWndMDIActive != nullptr) {
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

	int nMaxTitleLength;
	if (m_bAutoMaxWidth)
		nMaxTitleLength = static_cast<int>(MDITABBAR_MAXTITLELENGTH - (MDIFrameList.GetCount() - 1) * 6);
	else
		nMaxTitleLength = MDITABBAR_MAXTITLELENGTH;
	if (nMaxTitleLength < MDITABBAR_MINTITLELENGTH)
		nMaxTitleLength = MDITABBAR_MINTITLELENGTH;

	// Update or insert tabs
	for (POSITION pos = MDIFrameList.GetStartPosition(); pos != nullptr; )
	{
		HWND hFrameWnd;
//~		int item;
		MDIFrameList.GetNextAssoc(pos, hFrameWnd, item);

		CString strTitle;
		CDocument *pDoc = ((CFrameWnd *)FromHandle(hFrameWnd))->GetActiveDocument();
		if (pDoc != nullptr)
			strTitle = pDoc->GetTitle();
		else
			FromHandle(hFrameWnd)->GetWindowText(strTitle);

		if (strTitle.GetLength() > nMaxTitleLength)
			strTitle = strTitle.Left(nMaxTitleLength - 3) + _T("...");

		// Escape the '&' to prevent it from being removed and underlining the next character in the string.
		strTitle.Replace(_T("&"), _T("&&"));

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
			szText[0] = '\0';
			tci.pszText = szText;
			tci.cchTextMax = static_cast<int>(std::size(szText));
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
		if (!MDIFrameList.Lookup((HWND)tci.lParam, dummy))
		{
			DeleteItem(item);
			if (GetItemCount() == 0)
				m_pMainFrame->RecalcLayout();
		}
	}

	m_nTooltipTabItemIndex = -1;
}

/**
 * @brief Called when middle mouse button is pressed.
 * This function closes the tab when the middle mouse button is pressed.
 */
void CMDITabBar::OnMButtonDown(UINT nFlags, CPoint point)
{
	int index = GetItemIndexFromPoint(point);
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

	const int lpx = ::GetDeviceCaps(lpDraw->hDC, LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int r = pointToPixel(RR_RADIUS);
	const int sw = pointToPixel(RR_SHADOWWIDTH);

	CRect rc = lpDraw->rcItem;
	if (lpDraw->itemState & ODS_SELECTED)
	{
		const COLORREF clrShadow = CEColor::GetIntermediateColor(GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DFACE), 0.5f);
		if (GetSysColor(COLOR_3DFACE) == GetSysColor(COLOR_WINDOW))
		{
			DrawRoundedRectWithShadow(lpDraw->hDC, rc.left + sw, sw - 1, rc.Width() - sw * 2, rc.top - sw * 2 + 2, r, sw,
				GetSysColor(COLOR_HIGHLIGHT), clrShadow, GetSysColor(COLOR_3DFACE));
			SetTextColor(lpDraw->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else
		{
			DrawRoundedRectWithShadow(lpDraw->hDC, rc.left + sw, sw - 1, rc.Width() - sw * 2, rc.Height() - sw * 2 + 2, r, sw,
				GetSysColor(COLOR_WINDOW), clrShadow, GetSysColor(COLOR_3DFACE));
			SetTextColor(lpDraw->hDC, GetSysColor(COLOR_WINDOWTEXT));
		}
	}
	else
	{
		SetTextColor(lpDraw->hDC, GetSysColor(COLOR_BTNTEXT));
	}
	CSize iconsize(determineIconSize(), determineIconSize());
	rc.left += sw * 2 + iconsize.cx;
	SetBkMode(lpDraw->hDC, TRANSPARENT);
	HWND hwndFrame = reinterpret_cast<HWND>(item.lParam);
	if (::IsWindow(hwndFrame))
	{
		HICON hIcon = (HICON)::SendMessage(hwndFrame, WM_GETICON, ICON_SMALL2, 0);
		if (hIcon == nullptr)
			hIcon = (HICON)GetClassLongPtr(hwndFrame, GCLP_HICONSM);
		if (hIcon != nullptr)
			DrawIconEx(lpDraw->hDC, rc.left - iconsize.cx, rc.top + (rc.Height() - iconsize.cy) / 2, hIcon, iconsize.cx, iconsize.cy, 0, nullptr, DI_NORMAL);
	}
	rc.left += sw;
	DrawText(lpDraw->hDC, szBuf, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	int nItem = GetItemIndexFromPoint(m_rcCurrentCloseButtom.CenterPoint());
	if (static_cast<UINT>(nItem) == lpDraw->itemID)
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		CRect rc1 = GetCloseButtonRect(nItem);
		DrawFrameControl(lpDraw->hDC, &rc1, DFC_CAPTION, 
			DFCS_CAPTIONCLOSE | DFCS_FLAT | (rc1.PtInRect(pt) ? DFCS_HOT : 0) |
			((m_bCloseButtonDown && rc1.PtInRect(pt)) ? DFCS_PUSHED : 0));
	}
}

void CMDITabBar::OnMouseMove(UINT nFlags, CPoint point)
{
	int nTabItemIndex = GetItemIndexFromPoint(point);
	CRect rc = GetCloseButtonRect(nTabItemIndex);
	if (rc != m_rcCurrentCloseButtom)
	{
		InvalidateRect(&rc);
		InvalidateRect(&m_rcCurrentCloseButtom);
	}
	m_rcCurrentCloseButtom = rc;
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT };
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
	if (m_nDraggingTabItemIndex >= 0 && nTabItemIndex >= 0 && m_nDraggingTabItemIndex != nTabItemIndex)
	{
		CRect rectDraggingTab, rectDest;
		GetItemRect(m_nDraggingTabItemIndex, &rectDraggingTab);
		GetItemRect(nTabItemIndex, &rectDest);
		rectDest.right = rectDest.left + rectDraggingTab.Width();
		if (rectDest.PtInRect(point))
		{
			SwapTabs(m_nDraggingTabItemIndex, nTabItemIndex);
			m_nDraggingTabItemIndex = nTabItemIndex;
			m_rcCurrentCloseButtom = GetCloseButtonRect(nTabItemIndex);
			Invalidate();
		}
	}

	if (nTabItemIndex != m_nTooltipTabItemIndex)
		UpdateToolTips(nTabItemIndex);
}

void CMDITabBar::OnMouseLeave()
{
	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
	tme.dwFlags = TME_LEAVE | TME_CANCEL;
	tme.hwndTrack = m_hWnd;
	TrackMouseEvent(&tme);
	m_bMouseTracking = false;
	InvalidateRect(&m_rcCurrentCloseButtom);
	m_rcCurrentCloseButtom = CRect();
	m_bCloseButtonDown = false;
}

void CMDITabBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bCloseButtonDown = !!m_rcCurrentCloseButtom.PtInRect(point);
	InvalidateRect(m_rcCurrentCloseButtom);
	if (!m_bCloseButtonDown)
	{
        if (DragDetect(point))
		{
			m_nDraggingTabItemIndex = GetItemIndexFromPoint(point);
			SetCapture();
		}
		CWnd::OnLButtonDown(nFlags, point);
	}
}

void CMDITabBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nDraggingTabItemIndex >= 0)
	{
		m_nDraggingTabItemIndex = -1;
		ReleaseCapture();
	}
	else
	{
		if (m_bCloseButtonDown && m_rcCurrentCloseButtom.PtInRect(point))
			OnMButtonDown(nFlags, point);
		InvalidateRect(m_rcCurrentCloseButtom);
		m_bCloseButtonDown = false;
	}
	CWnd::OnLButtonUp(nFlags, point);
}

CRect CMDITabBar::GetCloseButtonRect(int nItem)
{
	CClientDC dc(this);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int r = pointToPixel(RR_RADIUS);
	const int sw = pointToPixel(RR_SHADOWWIDTH);
	CRect rc, rcClient;
	CSize size(determineIconSize(), determineIconSize());
	GetClientRect(&rcClient);
	GetItemRect(nItem, &rc);
	rc.left = rc.right - size.cx - sw - r;
	rc.right = rc.left + size.cx;
	int y = (rcClient.top + rcClient.bottom) / 2;
	rc.top = y - size.cy / 2 + 1;
	rc.bottom = rc.top + size.cy;
	return rc;
}

int CMDITabBar::GetItemIndexFromPoint(CPoint point) const
{
	TCHITTESTINFO hit;
	hit.pt = point;
	return HitTest(&hit);
}

void CMDITabBar::SwapTabs(int nIndexA, int nIndexB)
{
	TC_ITEM tciA = {0}, tciB = {0};
	TCHAR szTextA[256], szTextB[256];
	int nCurSel = GetCurSel();

	tciA.cchTextMax = sizeof(szTextA)/sizeof(szTextA[0]);
	tciB.cchTextMax = sizeof(szTextB)/sizeof(szTextB[0]);
	tciA.pszText = szTextA;
	tciB.pszText = szTextB;
	tciA.mask = tciB.mask = TCIF_PARAM | TCIF_TEXT;

	GetItem(nIndexA, &tciA);
	GetItem(nIndexB, &tciB);

	std::swap(tciA, tciB);

	SetItem(nIndexB, &tciB);
	SetItem(nIndexA, &tciA);

	if (nCurSel == nIndexA)
		SetCurSel(nIndexB);
	if (nCurSel == nIndexB)
		SetCurSel(nIndexA);
}

/**
 * @brief Get the maximum length of the title.
 */
int CMDITabBar::GetMaxTitleLength() const
{
	int nMaxTitleLength = m_bAutoMaxWidth ? static_cast<int>(MDITABBAR_MAXTITLELENGTH - (GetItemCount() - 1) * 6) : MDITABBAR_MAXTITLELENGTH;
	if (nMaxTitleLength < MDITABBAR_MINTITLELENGTH)
		nMaxTitleLength = MDITABBAR_MINTITLELENGTH;

	return nMaxTitleLength;
}

/**
 * @brief Update tooltip text.
 * @param [in] nTabItemIndex Index of the tab displaying tooltip.
 */
void CMDITabBar::UpdateToolTips(int nTabItemIndex)
{
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	GetItem(nTabItemIndex, &tci);

	if (!m_pMainFrame)
		return;
	CMDIChildWnd* pActiveWnd = m_pMainFrame->MDIGetActive();
	if (!pActiveWnd)
		return;
	CWnd* pParentWnd = pActiveWnd->GetParent();
	if (!pParentWnd)
		return;

	for (CWnd* pFrame = pParentWnd->GetTopWindow(); pFrame; pFrame = pFrame->GetNextWindow())
		if (reinterpret_cast<HWND>(tci.lParam) == pFrame->m_hWnd)
		{
			HWND hFrameWnd = pFrame->m_hWnd;
			CString strTitle, strTooltip;
			CFrameWnd* pFrameWnd = (CFrameWnd*)FromHandle(hFrameWnd);
			CDocument* pDoc = pFrameWnd->GetActiveDocument();
			IMDITab* pITabBar = nullptr;
			if (pDoc != nullptr)
			{
				strTitle = pDoc->GetTitle();
				pITabBar = dynamic_cast<IMDITab*>(pDoc);
			}
			else
			{
				pFrameWnd->GetWindowText(strTitle);
				pITabBar = dynamic_cast<IMDITab*>(pFrameWnd);
			}
			strTooltip = pITabBar ? pITabBar->GetTooltipString() : _T("");

			if (strTooltip == strTitle && strTitle.GetLength() <= GetMaxTitleLength())
				strTooltip.Empty();

			constexpr size_t MAX_TIP_TEXT_LENGTH = 1024;
			if (strTooltip.GetLength() > MAX_TIP_TEXT_LENGTH)
				strTooltip.Truncate(MAX_TIP_TEXT_LENGTH);

			m_tooltips.UpdateTipText(strTooltip, this);
			CRect rc;
			GetClientRect(&rc);
			m_tooltips.SetMaxTipWidth(rc.Width() * 60 / 100);
			m_nTooltipTabItemIndex = nTabItemIndex;
			return;
		}
}
