#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "ImgMergeWindow.hpp"
#include "WinIMergeLib.h"
#include "resource.h"

#pragma once

class CImgToolWindow : public IImgToolWindow
{
public:
	CImgToolWindow() :
		  m_hWnd(NULL)
		, m_hInstance(NULL)
		, m_pImgMergeWindow(NULL)
	{
	}

	~CImgToolWindow()
	{
	}

	bool Create(HINSTANCE hInstance, HWND hWndParent)
	{
		m_hInstance = hInstance;
		m_hWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOGBAR), hWndParent, DlgProc, reinterpret_cast<LPARAM>(this));
		return m_hWnd ? true : false;
	}

	bool Destroy()
	{
		BOOL bSucceeded = DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		return !!bSucceeded;
	}

	HWND GetHWND() const
	{
		return m_hWnd;
	}

	void Sync()
	{
		if (!m_pImgMergeWindow)
			return;
		TCHAR buf[256];
		wsprintf(buf, _T("(%d)"), m_pImgMergeWindow->GetDiffBlockSize());
		SetDlgItemText(m_hWnd, IDC_DIFF_BLOCKSIZE_STATIC, buf);
		wsprintf(buf, _T("(%d)"), static_cast<int>(m_pImgMergeWindow->GetDiffColorAlpha() * 100));
		SetDlgItemText(m_hWnd, IDC_DIFF_BLOCKALPHA_STATIC, buf);
		wsprintf(buf, _T("(%d)"), static_cast<int>(m_pImgMergeWindow->GetColorDistanceThreshold()));
		SetDlgItemText(m_hWnd, IDC_DIFF_CDTHRESHOLD_STATIC, buf);
		wsprintf(buf, _T("(%d)"), static_cast<int>(m_pImgMergeWindow->GetOverlayAlpha() * 100));
		SetDlgItemText(m_hWnd, IDC_OVERLAY_ALPHA_STATIC, buf);
		wsprintf(buf, _T("(%d%%)"), static_cast<int>(100 * m_pImgMergeWindow->GetZoom()));
		SetDlgItemText(m_hWnd, IDC_ZOOM_STATIC, buf);

		SendDlgItemMessage(m_hWnd, IDC_DIFF_HIGHLIGHT, BM_SETCHECK, m_pImgMergeWindow->GetShowDifferences() ? BST_CHECKED : BST_UNCHECKED, 0);
		SendDlgItemMessage(m_hWnd, IDC_DIFF_BLOCKSIZE_SLIDER, TBM_SETPOS, TRUE, m_pImgMergeWindow->GetDiffBlockSize());
		SendDlgItemMessage(m_hWnd, IDC_DIFF_BLOCKALPHA_SLIDER, TBM_SETPOS, TRUE, static_cast<LPARAM>(m_pImgMergeWindow->GetDiffColorAlpha() * 100));
		SendDlgItemMessage(m_hWnd, IDC_DIFF_CDTHRESHOLD_SLIDER, TBM_SETPOS, TRUE, static_cast<LPARAM>(m_pImgMergeWindow->GetColorDistanceThreshold()));
		SendDlgItemMessage(m_hWnd, IDC_OVERLAY_ALPHA_SLIDER, TBM_SETPOS, TRUE, static_cast<LPARAM>(m_pImgMergeWindow->GetOverlayAlpha() * 100));
		SendDlgItemMessage(m_hWnd, IDC_ZOOM_SLIDER, TBM_SETPOS, TRUE, static_cast<LPARAM>(m_pImgMergeWindow->GetZoom() * 8 - 8));
		SendDlgItemMessage(m_hWnd, IDC_OVERLAY_MODE, CB_SETCURSEL, m_pImgMergeWindow->GetOverlayMode(), 0);
		SendDlgItemMessage(m_hWnd, IDC_PAGE_SPIN, UDM_SETRANGE, 0, MAKELONG(1, m_pImgMergeWindow->GetMaxPageCount()));
		SendDlgItemMessage(m_hWnd, IDC_PAGE_SPIN, UDM_SETPOS, 0, MAKELONG(m_pImgMergeWindow->GetCurrentMaxPage() + 1, 0));

		int w = static_cast<CImgMergeWindow *>(m_pImgMergeWindow)->GetDiffImageWidth();
		int h = static_cast<CImgMergeWindow *>(m_pImgMergeWindow)->GetDiffImageHeight();

		RECT rc;
		GetClientRect(m_hWnd, &rc);
		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;

		RECT rcTmp;
		HWND hwndDiffMap = GetDlgItem(m_hWnd, IDC_DIFFMAP);
		GetWindowRect(hwndDiffMap, &rcTmp);
		POINT pt = { rcTmp.left, rcTmp.top };
		ScreenToClient(m_hWnd, &pt);
		int mh = h * (cx - 8) / w;
		if (mh + pt.y > cy - 8)
			mh = cy - 8 - pt.y;
		int mw = mh * w / h;
		RECT rcDiffMap = { (cx - mw) / 2, pt.y, (cx + mw) / 2, pt.y + mh };
		SetWindowPos(hwndDiffMap, NULL, rcDiffMap.left, rcDiffMap.top, 
			rcDiffMap.right - rcDiffMap.left, rcDiffMap.bottom - rcDiffMap.top, SWP_NOZORDER);

		InvalidateRect(GetDlgItem(m_hWnd, IDC_DIFFMAP), NULL, TRUE);
	}

	void SetImgMergeWindow(IImgMergeWindow *pImgMergeWindow)
	{
		m_pImgMergeWindow = pImgMergeWindow;
		m_pImgMergeWindow->AddEventListener(OnEvent, this);
	}

private:
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
	{
		SendDlgItemMessage(hwnd, IDC_DIFF_BLOCKSIZE_SLIDER, TBM_SETRANGE, TRUE, MAKELPARAM(1, 64));
		SendDlgItemMessage(hwnd, IDC_DIFF_BLOCKALPHA_SLIDER, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		SendDlgItemMessage(hwnd, IDC_OVERLAY_ALPHA_SLIDER, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		SendDlgItemMessage(hwnd, IDC_ZOOM_SLIDER, TBM_SETRANGE, TRUE, MAKELPARAM(-7, 56));
		SendDlgItemMessage(hwnd, IDC_OVERLAY_MODE, CB_ADDSTRING, 0, (LPARAM)(_T("None")));
		SendDlgItemMessage(hwnd, IDC_OVERLAY_MODE, CB_ADDSTRING, 0, (LPARAM)(_T("XOR")));
		SendDlgItemMessage(hwnd, IDC_OVERLAY_MODE, CB_ADDSTRING, 0, (LPARAM)(_T("Alpha")));
		SendDlgItemMessage(hwnd, IDC_OVERLAY_MODE, CB_ADDSTRING, 0, (LPARAM)(_T("Alpha Animation")));
		return TRUE;
	}

	void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
	{
		switch (id)
		{
		case IDC_DIFF_HIGHLIGHT:
			if (codeNotify == BN_CLICKED)
				m_pImgMergeWindow->SetShowDifferences(Button_GetCheck(hwndCtl) == BST_CHECKED);
			break;
		case IDC_DIFF_BLINK:
			if (codeNotify == BN_CLICKED)
				m_pImgMergeWindow->SetBlinkDifferences(Button_GetCheck(hwndCtl) == BST_CHECKED);
			break;
		case IDC_OVERLAY_MODE:
			if (codeNotify == CBN_SELCHANGE)
				m_pImgMergeWindow->SetOverlayMode(static_cast<IImgMergeWindow::OVERLAY_MODE>(ComboBox_GetCurSel(hwndCtl)));
			break;
		case IDC_PAGE_EDIT:
			if (codeNotify == EN_CHANGE)
			{
				int page = static_cast<int>(SendDlgItemMessage(hwnd, IDC_PAGE_SPIN, UDM_GETPOS, 0, 0));
				m_pImgMergeWindow->SetCurrentPageAll(page - 1);
			}
			break;
		case IDC_DIFFMAP:
			if (codeNotify == STN_CLICKED)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hwndCtl, &pt);
				RECT rc;
				GetClientRect(hwndCtl, &rc);
				CImgMergeWindow *pImgMergeWindow = static_cast<CImgMergeWindow *>(m_pImgMergeWindow);
				pImgMergeWindow->ScrollTo(
					pt.x * pImgMergeWindow->GetDiffImageWidth() / rc.right,
					pt.y * pImgMergeWindow->GetDiffImageHeight() / rc.bottom,
					true);
			}
			break;
		}
	}

	void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
	{
		int val = static_cast<int>(SendMessage(hwndCtl, TBM_GETPOS, 0, 0));
		switch (GetDlgCtrlID(hwndCtl))
		{
		case IDC_DIFF_BLOCKALPHA_SLIDER:
			m_pImgMergeWindow->SetDiffColorAlpha(val / 100.0);
			break;
		case IDC_DIFF_BLOCKSIZE_SLIDER:
			m_pImgMergeWindow->SetDiffBlockSize(val);
			break;
		case IDC_DIFF_CDTHRESHOLD_SLIDER:
			m_pImgMergeWindow->SetColorDistanceThreshold(val);
			break;
		case IDC_OVERLAY_ALPHA_SLIDER:
			m_pImgMergeWindow->SetOverlayAlpha(val / 100.0);
			break;
		case IDC_ZOOM_SLIDER:
			m_pImgMergeWindow->SetZoom(1.0 + val * 0.125);
			break;
		}
		Sync();
	}

	void OnSize(HWND hwnd, UINT nType, int cx, int cy)
	{
		int nIDs[] = {
			IDC_DIFF_GROUP,
			IDC_OVERLAY_GROUP,
			IDC_VIEW_GROUP,
			IDC_DIFF_BLOCKALPHA_SLIDER,
			IDC_DIFF_BLOCKSIZE_SLIDER,
			IDC_DIFF_CDTHRESHOLD_SLIDER,
			IDC_OVERLAY_ALPHA_SLIDER,
			IDC_ZOOM_SLIDER
		};

		RECT rc;
		GetClientRect(m_hWnd, &rc);
		for (int i = 0; i < sizeof(nIDs) / sizeof(nIDs[0]); ++i)
		{
			RECT rcCtrl;
			HWND hwndCtrl = GetDlgItem(m_hWnd, nIDs[i]);
			GetWindowRect(hwndCtrl, &rcCtrl);
			POINT pt = { rcCtrl.left, rcCtrl.top };
			ScreenToClient(m_hWnd, &pt);
			MoveWindow(hwndCtrl, pt.x, pt.y, rc.right - pt.x * 2, rcCtrl.bottom - rcCtrl.top, TRUE);
		}

		Sync();
	}

	void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *pDrawItem)
	{
		if (!m_pImgMergeWindow || m_pImgMergeWindow->GetPaneCount() == 0)
			return;
		RECT rc;
		GetClientRect(pDrawItem->hwndItem, &rc);
		Image *pImage = static_cast<CImgMergeWindow *>(m_pImgMergeWindow)->GetDiffMapImage(rc.right - rc.left, rc.bottom - rc.top);
		RGBQUAD bkColor = { 0xff, 0xff, 0xff, 0xff };
		pImage->getFipImage()->drawEx(pDrawItem->hDC, rc, false, &bkColor);
		HWND hwndLeftPane = m_pImgMergeWindow->GetPaneHWND(0);

		SCROLLINFO sih, siv;
		sih.cbSize = sizeof(sih);
		sih.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
		GetScrollInfo(hwndLeftPane, SB_HORZ, &sih);
		siv.cbSize = sizeof(siv);
		siv.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
		GetScrollInfo(hwndLeftPane, SB_VERT, &siv);

		if (static_cast<int>(sih.nPage) < sih.nMax || static_cast<int>(siv.nPage) < siv.nMax)
		{
			RECT rcFrame;
			rcFrame.left = rc.left + (rc.right - rc.left) * sih.nPos / sih.nMax;
			rcFrame.right = rcFrame.left + (rc.right - rc.left) * sih.nPage / sih.nMax;
			rcFrame.top = rc.top + (rc.bottom - rc.top) * siv.nPos / siv.nMax;
			rcFrame.bottom = rcFrame.top + (rc.bottom - rc.top) * siv.nPage / siv.nMax;
			FrameRect(pDrawItem->hDC, &rcFrame, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
		}
	}

	INT_PTR OnWndMsg(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (iMsg)
		{
		case WM_INITDIALOG:
			return HANDLE_WM_INITDIALOG(hwnd, wParam, lParam, OnInitDialog);
		case WM_COMMAND:
			HANDLE_WM_COMMAND(hwnd, wParam, lParam, OnCommand);
			break;
		case WM_HSCROLL:
			HANDLE_WM_HSCROLL(hwnd, wParam, lParam, OnHScroll);
			break;
		case WM_SIZE:
			HANDLE_WM_SIZE(hwnd, wParam, lParam, OnSize);
			break;
		case WM_DRAWITEM:
			HANDLE_WM_DRAWITEM(hwnd, wParam, lParam, OnDrawItem);
			break;
		}
		return 0;
	}

	static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		if (iMsg == WM_INITDIALOG)
			SetWindowLongPtr(hwnd, DWLP_USER, reinterpret_cast<LONG_PTR>(reinterpret_cast<CImgToolWindow *>(lParam)));
		CImgToolWindow *pImgWnd = reinterpret_cast<CImgToolWindow *>(GetWindowLongPtr(hwnd, DWLP_USER));
		if (pImgWnd)
			return pImgWnd->OnWndMsg(hwnd, iMsg, wParam, lParam);
		else
			return FALSE;
	}

	static void OnEvent(const IImgMergeWindow::Event& evt)
	{
		switch (evt.eventType)
		{
		case IImgMergeWindow::HSCROLL:
		case IImgMergeWindow::VSCROLL:
		case IImgMergeWindow::SIZE:
		case IImgMergeWindow::MOUSEWHEEL:
		case IImgMergeWindow::REFRESH:
		case IImgMergeWindow::SCROLLTODIFF:
		case IImgMergeWindow::OPEN:
			reinterpret_cast<CImgToolWindow *>(evt.userdata)->Sync();
			break;
		}
	}

	HWND m_hWnd;
	HINSTANCE m_hInstance;
	IImgMergeWindow *m_pImgMergeWindow;
};
