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
constexpr int RR_PADDING = 3;
constexpr int RR_SHADOWWIDTH = 3;

/////////////////////////////////////////////////////////////////////////////
// CMDITabBar

IMPLEMENT_DYNAMIC(CMyTabCtrl, CTabCtrl)

BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CMyTabCtrl)
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

IMPLEMENT_DYNAMIC(CMDITabBar, CControlBar)

BEGIN_MESSAGE_MAP(CMDITabBar, CControlBar)
	//{{AFX_MSG_MAP(CMDITabBar)
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCMOUSELEAVE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_NCRBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static int determineIconSize()
{
	return GetSystemMetrics(SM_CXSMICON);
}

BOOL CMyTabCtrl::Create(CMDIFrameWnd* pMainFrame, CWnd* pParent)
{
	if (!CTabCtrl::Create(WS_CHILD | WS_VISIBLE | TCS_OWNERDRAWFIXED, CRect(0, 0, 0, 0), pParent, 0))
		return FALSE;

	m_pMainFrame = pMainFrame;
	m_tooltips.Create(m_pMainFrame, TTS_NOPREFIX);
	m_tooltips.AddTool(this, _T(""));
	return TRUE;
}

/**
 * @brief Called before messages are translated.
 * Passes a mouse message to the ToolTip control for processing.
 * @param [in] pMsg Points to an MSG structure that contains the message to be chcecked
 */
BOOL CMyTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
		m_tooltips.RelayEvent(pMsg);

	// Call the parent method.
	return __super::PreTranslateMessage(pMsg);
}

void CMyTabCtrl::SetActive(bool bActive)
{
	CTitleBarHelper::ReloadAccentColor();
	m_bActive = bActive;
}

static inline COLORREF getTextColor()
{
	return GetSysColor(COLOR_WINDOWTEXT);
}

COLORREF CMyTabCtrl::GetBackColor() const
{
	const COLORREF clr = GetSysColor(COLOR_3DFACE);
	if (!m_bOnTitleBar)
		return clr;
	return CTitleBarHelper::GetBackColor(m_bActive);
}

static inline bool IsHighContrastEnabled()

{
	HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0);
	return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

void CMyTabCtrl::OnPaint() 
{
	CPaintDC dc(this);
	dc.SelectObject(GetFont());

	DRAWITEMSTRUCT dis;
	dis.hDC = dc.GetSafeHdc();

	CRect rcClient;
	GetClientRect(&rcClient);

	const int nCount = GetItemCount();
	if (nCount == 0)
	{
		const COLORREF winTitleTextColor = m_bOnTitleBar ?
			CTitleBarHelper::GetTextColor(m_bActive) : getTextColor();
		dc.SetTextColor(winTitleTextColor);
		TCHAR szBuf[256];
		AfxGetMainWnd()->GetWindowText(szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
		dc.DrawText(szBuf, -1, &rcClient, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	int nCurSel = GetCurSel();
	for (int i = nCount - 1; i >= 0; --i)
	{
		GetItemRect(i, &dis.rcItem);
		dis.itemID = i;
		dis.rcItem.top = rcClient.top;
		dis.rcItem.bottom = rcClient.bottom;
		dis.itemState = (i != nCurSel) ? 0 : ODS_SELECTED;
		DrawItem(&dis);
	}
}

BOOL CMyTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rClient;
	GetClientRect(rClient);
	pDC->FillSolidRect(rClient, GetBackColor());
	return TRUE;
}

/** 
 * @brief Called when tab selection is changed.
 */
BOOL CMyTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
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
void CMyTabCtrl::OnContextMenu(CWnd *pWnd, CPoint point)
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
void CMyTabCtrl::UpdateTabs()
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
void CMyTabCtrl::OnMButtonDown(UINT nFlags, CPoint point)
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

void CMyTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	TCHAR            szBuf[256];
	TCITEM           item;
	LPDRAWITEMSTRUCT lpDraw = (LPDRAWITEMSTRUCT)lpDrawItemStruct;

	item.mask       = TCIF_TEXT | TCIF_PARAM;
	item.pszText    = szBuf;
	item.cchTextMax = sizeof(szBuf) / sizeof(TCHAR);
	GetItem(lpDraw->itemID, &item);

	const int lpx = ::GetDeviceCaps(lpDraw->hDC, LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int r = pointToPixel(RR_RADIUS);
	const int pd = pointToPixel(RR_PADDING);
	const int sw = pointToPixel(RR_SHADOWWIDTH);

	CRect rc = lpDraw->rcItem;
	if (lpDraw->itemState & ODS_SELECTED)
	{
		const COLORREF clrShadow = CEColor::GetIntermediateColor(GetSysColor(COLOR_3DSHADOW), GetBackColor(), 0.5f);
		if (IsHighContrastEnabled())
		{
			DrawRoundedRectWithShadow(lpDraw->hDC, rc.left + sw, rc.top + sw - 1, rc.Width() - sw * 2, rc.Height() - rc.top - sw * 2 + 2, r, sw,
				GetSysColor(COLOR_HIGHLIGHT), clrShadow, GetBackColor());
			SetTextColor(lpDraw->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else
		{
			DrawRoundedRectWithShadow(lpDraw->hDC, rc.left + sw, rc.top + sw - 1, rc.Width() - sw * 2, rc.Height() - sw * 2 + 2, r, sw,
				GetSysColor(COLOR_3DHIGHLIGHT), clrShadow, GetBackColor());
			SetTextColor(lpDraw->hDC, getTextColor());
		}
	}
	else
	{
		const COLORREF txtclr = m_bOnTitleBar ?
			CTitleBarHelper::GetTextColor(m_bActive) : GetSysColor(COLOR_BTNTEXT);
		SetTextColor(lpDraw->hDC, txtclr);
	}
	CSize iconsize(determineIconSize(), determineIconSize());
	rc.left += sw + pd + iconsize.cx;
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
	rc.left += pd;
	rc.right -= pd;
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

void CMyTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
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

void CMyTabCtrl::OnMouseLeave()
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

void CMyTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
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

void CMyTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
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

CRect CMyTabCtrl::GetCloseButtonRect(int nItem)
{
	CClientDC dc(this);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int pd = pointToPixel(RR_PADDING);
	const int sw = pointToPixel(RR_SHADOWWIDTH);
	CRect rc, rcClient;
	CSize size(determineIconSize(), determineIconSize());
	GetClientRect(&rcClient);
	GetItemRect(nItem, &rc);
	rc.left = rc.right - size.cx - sw - pd;
	rc.right = rc.left + size.cx;
	int y = (rcClient.top + rcClient.bottom) / 2;
	rc.top = y - size.cy / 2 + 1;
	rc.bottom = rc.top + size.cy;
	return rc;
}

int CMyTabCtrl::GetItemIndexFromPoint(CPoint point) const
{
	TCHITTESTINFO hit;
	hit.pt = point;
	return HitTest(&hit);
}

void CMyTabCtrl::SwapTabs(int nIndexA, int nIndexB)
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
int CMyTabCtrl::GetMaxTitleLength() const
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
void CMyTabCtrl::UpdateToolTips(int nTabItemIndex)
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
	{
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
}

BOOL CMDITabBar::Update(bool bOnTitleBar, bool bMaximized)
{
	m_bOnTitleBar = bOnTitleBar;
	m_titleBar.SetMaximized(bMaximized);
	m_tabCtrl.SetOnTitleBar(bOnTitleBar);
	return true;
}

void CMDITabBar::UpdateActive(bool bActive)
{
	if (m_tabCtrl.GetActive() != bActive)
	{
		m_tabCtrl.SetActive(bActive);
		Invalidate();
	}
}

/** 
 * @brief Create tab bar.
 * @param pParentWnd [in] main frame window pointer
 */
BOOL CMDITabBar::Create(CMDIFrameWnd* pMainFrame)
{
	m_dwStyle = CBRS_TOP;

	m_titleBar.Init(this);

	CWnd::Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pMainFrame, AFX_IDW_CONTROLBAR_LAST - 1);

	if (!m_tabCtrl.Create(pMainFrame, this))
		return FALSE;

	CClientDC dc(this);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int pd = pointToPixel(RR_PADDING);
	const int sw = pointToPixel(RR_SHADOWWIDTH);
	m_tabCtrl.SetPadding(CSize(sw + pd * 2 + determineIconSize() / 2, sw + pd));

	NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0);
	m_font.CreateFontIndirect(&ncm.lfMenuFont);
	m_tabCtrl.SetFont(&m_font);

	return TRUE;
}

/** 
 * @brief This method calculates the horizontal size of a control bar.
 */
CSize CMDITabBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (!m_bOnTitleBar && m_tabCtrl.GetItemCount() == 0)
		return CSize(SHRT_MAX, 0);
	
	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_font);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);

	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int pd = pointToPixel(RR_PADDING);
	const int sw = pointToPixel(RR_SHADOWWIDTH);
	int my = m_bOnTitleBar ? (m_titleBar.GetTopMargin() + 2) : 0;
	CSize size(SHRT_MAX, my + tm.tmHeight + (sw + pd) * 2);
	return size;
}

LRESULT CMDITabBar::OnNcHitTest(CPoint point)
{
	if (!m_bOnTitleBar)
		return __super::OnNcHitTest(point);
	return m_titleBar.OnNcHitTest(point);
}

void CMDITabBar::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	m_titleBar.OnNcMouseMove(nHitTest, point);
}

void CMDITabBar::OnNcMouseLeave()
{
	m_titleBar.OnNcMouseLeave();
}

void CMDITabBar::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	m_titleBar.OnNcLButtonDblClk(nHitTest, point);
}

void CMDITabBar::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	m_titleBar.OnNcLButtonDown(nHitTest, point);
}

void CMDITabBar::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	m_titleBar.OnNcLButtonUp(nHitTest, point);
}

void CMDITabBar::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	m_titleBar.OnNcRButtonDown(nHitTest, point);
}

void CMDITabBar::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	m_titleBar.OnNcRButtonUp(nHitTest, point);
}

void CMDITabBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	m_titleBar.SetSize(cx, cy);
	if (m_tabCtrl.m_hWnd)
	{
		const int leftMargin = m_bOnTitleBar ? m_titleBar.GetLeftMargin() : 0;
		const int rightMargin = m_bOnTitleBar ? m_titleBar.GetRightMargin() : 0;
		const int topMargin = ((m_titleBar.GetMaximized() && m_bOnTitleBar) ? m_titleBar.GetTopMargin() : 0) + (m_bOnTitleBar ? 1 : 0);
		const int bottomMargin = m_bOnTitleBar ? 1 : 0;
		CSize size{ 0, cy - topMargin - bottomMargin };
		m_tabCtrl.MoveWindow(leftMargin, topMargin, cx - leftMargin - rightMargin, cy - topMargin - bottomMargin, true);
		m_tabCtrl.SetItemSize(size);
	}
}

BOOL CMDITabBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rClient;
	GetClientRect(rClient);
	pDC->FillSolidRect(rClient, m_tabCtrl.GetBackColor());
	return TRUE;
}

void CMDITabBar::OnPaint()
{
	if (!m_bOnTitleBar)
		return __super::OnPaint();
	CPaintDC dc(this);
	m_titleBar.DrawIcon(AfxGetMainWnd(), dc, m_tabCtrl.GetActive());
	m_titleBar.DrawButtons(dc, CTitleBarHelper::GetTextColor(m_tabCtrl.GetActive()), m_tabCtrl.GetBackColor());
}
