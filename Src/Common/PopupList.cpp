#include "StdAfx.h"
#include "PopupList.h"
#include "DarkModeLib.h"

namespace PopupList
{
namespace
{
/** @brief Popup host with a theme-aware client border */
class CPopupListWnd : public CWnd
{
public:
	void SetListBox(HWND hList) { m_hList = hList; }
protected:
	HWND m_hList = nullptr;
	HBRUSH GetPopupBackgroundBrush(HDC hDC);
	void FillPopupBackground(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPopupListWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_PRINTCLIENT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH CPopupListWnd::GetPopupBackgroundBrush(HDC hDC)
{
	HBRUSH hBackground = m_hList == nullptr ? nullptr : reinterpret_cast<HBRUSH>(SendMessage(
		WM_CTLCOLORLISTBOX, reinterpret_cast<WPARAM>(hDC), reinterpret_cast<LPARAM>(m_hList)));
	return hBackground != nullptr ? hBackground : GetSysColorBrush(COLOR_WINDOW);
}

void CPopupListWnd::FillPopupBackground(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	::FillRect(pDC->GetSafeHdc(), &rcClient, GetPopupBackgroundBrush(pDC->GetSafeHdc()));
}

void CPopupListWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(&rcClient);
	FillPopupBackground(&dc);
	const COLORREF borderColor = DarkMode::isEnabled() ? RGB(80, 80, 80) : GetSysColor(COLOR_3DSHADOW);
	CBrush borderBrush(borderColor);
	dc.FrameRect(&rcClient, &borderBrush);
}

BOOL CPopupListWnd::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

LRESULT CPopupListWnd::OnPrintClient(CDC* pDC, UINT nFlags)
{
	if (nFlags & PRF_CLIENT)
		FillPopupBackground(pDC);
	return 0;
}

HBRUSH CPopupListWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_BTN)
		return GetPopupBackgroundBrush(pDC->GetSafeHdc());
	return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

/**
 * @brief Show a popup list at ptScreen and run a modal loop like TrackPopupMenu.
 * The list is at most half the work area of the monitor at ptScreen high and has a scroll bar when the items
 * do not fit. The list box handles its normal mouse, wheel, and navigation behavior. A click chooses the item
 * under the pointer, Enter the selected one; Esc, Alt, a click outside (not passed on) or another active window
 * closes it.
 * @param [in] pFont Font of the list, nullptr for the default
 * @return Index of the chosen item, -1 for none (always -1 when bEnabled is false)
 */
int TrackPopupList(CWnd* pOwner, CFont* pFont, const std::vector<String>& items, int nSel, bool bEnabled,
	const String& checkBoxText, bool& bCheckBoxChecked, CPoint ptScreen)
{
	MONITORINFO monitorInfo{ sizeof(monitorInfo) };
	GetMonitorInfo(MonitorFromPoint(ptScreen, MONITOR_DEFAULTTONEAREST), &monitorInfo);
	const CRect rcWork(monitorInfo.rcWork);
	const int cyMax = rcWork.Height() / 2;

	CPopupListWnd wndPopup;
	const HBRUSH hPopupBackground = DarkMode::isEnabled() ? DarkMode::getDlgBackgroundBrush() : nullptr;
	if (!wndPopup.CreateEx(WS_EX_TOOLWINDOW, AfxRegisterWndClass(CS_DROPSHADOW, nullptr, hPopupBackground), nullptr,
			WS_POPUP, CRect(0, 0, 0, 0), pOwner, 0))
		return -1;
	CListBox list;
	if (!list.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, CRect(0, 0, 0, 0), &wndPopup, 1))
	{
		wndPopup.DestroyWindow();
		return -1;
	}
	CButton checkBox;
	if (!checkBox.Create(checkBoxText.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		CRect(0, 0, 0, 0), &wndPopup, 2))
	{
		wndPopup.DestroyWindow();
		return -1;
	}
	checkBox.SetFont(pFont);
	if (DarkMode::isEnabled())
	{
		// colors the list like a dialog control
		DarkMode::setWindowCtlColorSubclass(wndPopup.m_hWnd);
		DarkMode::setChildCtrlsSubclassAndThemeEx(wndPopup.m_hWnd, true, true);
	}
	wndPopup.SetListBox(list.m_hWnd);
	if (pFont != nullptr)
		list.SetFont(pFont);

	// Size: the widest item plus a margin, rows up to cyMax, plus the scroll bar the list then shows
	int cxText = 0, cxMargin = 0;
	{
		CClientDC dc(&list);
		const HFONT hFont = reinterpret_cast<HFONT>(list.SendMessage(WM_GETFONT));
		const HGDIOBJ hOldFont = hFont ? dc.SelectObject(hFont) : nullptr;
		for (const auto& item : items)
		{
			list.AddString(item.c_str());
			cxText = (std::max)(cxText, static_cast<int>(dc.GetTextExtent(item.c_str()).cx));
		}
		cxMargin = dc.GetTextExtent(_T("  ")).cx;
		if (hOldFont)
			dc.SelectObject(hOldFont);
	}
	const int cyItem = (std::max)(1, list.GetItemHeight(0));
	const int nItems = static_cast<int>(items.size());
	const int nRows = (std::max)(1, (std::min)(nItems, cyMax / cyItem));
	CSize sizeCheckText;
	{
		CClientDC dc(&checkBox);
		const HFONT hFont = reinterpret_cast<HFONT>(checkBox.SendMessage(WM_GETFONT));
		const HGDIOBJ hOldFont = hFont ? dc.SelectObject(hFont) : nullptr;
		sizeCheckText = dc.GetTextExtent(checkBoxText.c_str());
		if (hOldFont)
			dc.SelectObject(hOldFont);
	}
	const int cyCheck = (std::max)(GetSystemMetrics(SM_CYMENUCHECK), static_cast<int>(sizeCheckText.cy)) + 6;
	const int cxLeftMargin = 6;
	const int cxRightMargin = 1;
	list.MoveWindow(cxLeftMargin, cyCheck, cxText + cxMargin, nRows * cyItem);
	CRect rcWindow, rcClient;
	list.GetWindowRect(&rcWindow);
	list.GetClientRect(&rcClient);
	const CSize sizeList(cxText + cxMargin + rcWindow.Width() - rcClient.Width(), nRows * cyItem);
	const int cxPopup = (std::max)(static_cast<int>(sizeList.cx) + cxLeftMargin + cxRightMargin,
		static_cast<int>(sizeCheckText.cx) + GetSystemMetrics(SM_CXMENUCHECK) + 20);
	list.MoveWindow(cxLeftMargin, cyCheck, cxPopup - cxLeftMargin - cxRightMargin, sizeList.cy);
	checkBox.MoveWindow(cxLeftMargin, 2, cxPopup - cxLeftMargin - 6, cyCheck - 2);

	// Place it like a popup menu: above the point, below it when the work area has no room above,
	// and inside the work area
	CRect rcPopup(CPoint(0, 0), CSize(cxPopup, sizeList.cy + cyCheck));
	::AdjustWindowRectEx(&rcPopup, WS_POPUP, FALSE, WS_EX_TOOLWINDOW);
	const CSize sizePopup = rcPopup.Size();
	CPoint ptPopup(ptScreen.x, ptScreen.y - sizePopup.cy);
	if (ptPopup.y < rcWork.top)
		ptPopup.y = ptScreen.y;
	ptPopup.x = (std::max)(rcWork.left, (std::min)(ptPopup.x, rcWork.right - sizePopup.cx));
	ptPopup.y = (std::max)(rcWork.top, (std::min)(ptPopup.y, rcWork.bottom - sizePopup.cy));
	list.SetCurSel(nSel);
	checkBox.SetCheck(bCheckBoxChecked ? BST_CHECKED : BST_UNCHECKED);
	list.EnableWindow(bEnabled);
	wndPopup.SetWindowPos(&CWnd::wndTop, ptPopup.x, ptPopup.y, sizePopup.cx, sizePopup.cy, SWP_SHOWWINDOW);
	if (bEnabled)
		list.SetFocus();
	wndPopup.SetTimer(1, 100, nullptr); // wakes the loop to notice another active window

	int nResult = -1;
	// Whether message is the button up (client or non-client) of the button of downMessage
	auto isButtonUp = [](UINT message, UINT downMessage) {
		switch (downMessage)
		{
		case WM_LBUTTONDOWN: case WM_NCLBUTTONDOWN: return message == WM_LBUTTONUP || message == WM_NCLBUTTONUP;
		case WM_RBUTTONDOWN: case WM_NCRBUTTONDOWN: return message == WM_RBUTTONUP || message == WM_NCRBUTTONUP;
		case WM_MBUTTONDOWN: case WM_NCMBUTTONDOWN: return message == WM_MBUTTONUP || message == WM_NCMBUTTONUP;
		}
		return false;
	};
	UINT nOutsideDown = 0; // button down of a click outside, which closes the list
	for (bool bDone = false; !bDone; )
	{
		if (::GetActiveWindow() != wndPopup.m_hWnd || (nOutsideDown != 0 && ::GetCapture() != wndPopup.m_hWnd))
			break;
		MSG msg;
		if (!::GetMessage(&msg, nullptr, 0, 0))
		{
			::PostQuitMessage(static_cast<int>(msg.wParam));
			break;
		}
		if (nOutsideDown != 0)
		{
			// Closing after a click outside: its mouse input up to the button up is dropped, not passed on
			if (isButtonUp(msg.message, nOutsideDown))
				bDone = true;
			if ((msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) ||
				(msg.message >= WM_NCMOUSEMOVE && msg.message <= WM_NCMBUTTONDBLCLK))
				continue;
		}
		bool bDispatch = true;
		switch (msg.message)
		{
		case WM_KEYDOWN:
			if (msg.wParam == VK_ESCAPE)
				bDone = true;
			else if (msg.wParam == VK_RETURN)
			{
				nResult = bEnabled ? list.GetCurSel() : -1;
				bDone = true;
			}
			break;
		case WM_SYSKEYDOWN:
			bDone = true;
			break;
		case WM_MOUSEWHEEL:
			// Route wheel input to the list box, which applies the Windows wheel settings itself.
			bDispatch = false;
			if (bEnabled)
				list.SendMessage(WM_MOUSEWHEEL, msg.wParam, msg.lParam);
			break;
		case WM_LBUTTONUP:
			if (msg.hwnd == list.m_hWnd && bEnabled)
			{
				CRect rcClient;
				list.GetClientRect(&rcClient);
				const CPoint ptClient(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
				BOOL bOutside = TRUE;
				list.ItemFromPoint(ptClient, bOutside);
				if (!bOutside && rcClient.PtInRect(ptClient))
				{
					nResult = list.GetCurSel();
					bDone = true;
				}
			}
			break;
		case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
		case WM_NCLBUTTONDOWN: case WM_NCRBUTTONDOWN: case WM_NCMBUTTONDOWN:
			if (msg.hwnd != list.m_hWnd && msg.hwnd != checkBox.m_hWnd && msg.hwnd != wndPopup.m_hWnd)
			{
				// A click outside closes the list and is not passed on, like for a menu: the popup takes the
				// mouse until the button up, which is dropped as well
				nOutsideDown = msg.message;
				wndPopup.SetCapture();
				bDispatch = false;
			}
			break;
		}
		if (bDispatch && !bDone)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	bCheckBoxChecked = checkBox.GetCheck() == BST_CHECKED;
	wndPopup.DestroyWindow();
	return nResult;
}
}

Selection Show(CWnd* pOwner, CFont* pFont, const std::vector<String>& items, int selectedIndex,
	bool enabled, const String& checkBoxText, bool checkBoxChecked, CPoint ptScreen)
{
	Selection result;
	result.checkBoxChecked = checkBoxChecked;
	result.index = TrackPopupList(pOwner, pFont, items, selectedIndex, enabled, checkBoxText,
		result.checkBoxChecked, ptScreen);
	return result;
}
}
