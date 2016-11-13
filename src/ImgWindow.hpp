/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable: 4819)

#include "FreeImagePlus.h"
#include <vector>

class CImgWindow
{
	enum { MARGIN = 16 };
public:
	CImgWindow() :
		  m_fip(NULL)
		, m_hWnd(NULL)
		, m_nVScrollPos(0)
		, m_nHScrollPos(0)
		, m_zoom(1.0)
		, m_useBackColor(true)
	{
		memset(&m_backColor, 0xff, sizeof(m_backColor));
	}

	~CImgWindow()
	{
	}

	HWND GetHWND() const
	{
		return m_hWnd;
	}

	bool Create(HINSTANCE hInstance, HWND hWndParent)
	{
		MyRegisterClass(hInstance);
		m_hWnd = CreateWindowExW(0, L"WinImgWindowClass", NULL, WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE,
			0, 0, 0, 0, hWndParent, NULL, hInstance, this);
		return m_hWnd ? true : false;
	}

	bool Destroy()
	{
		if (m_hWnd)
			DestroyWindow(m_hWnd);
		m_fip = NULL;
		m_hWnd = NULL;
		return true;
	}

	RECT GetWindowRect() const
	{
		RECT rc, rcParent;
		HWND hwndParent = GetParent(m_hWnd);
		::GetWindowRect(hwndParent, &rcParent);
		::GetWindowRect(m_hWnd, &rc);
		rc.left   -= rcParent.left;
		rc.top    -= rcParent.top;
		rc.right  -= rcParent.left;
		rc.bottom -= rcParent.top;
		return rc;
	}

	void SetWindowRect(const RECT& rc)
	{
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	}

	void SetFocus()
	{
		::SetFocus(m_hWnd);
	}

	POINT ConvertDPtoLP(int dx, int dy) const
	{
		POINT lp;
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		if (rc.right - rc.left < m_fip->getWidth() * m_zoom + MARGIN * 2)
			lp.x = static_cast<int>((dx - MARGIN + m_nHScrollPos) / m_zoom);
		else
			lp.x = static_cast<int>((dx - (rc.right / 2 - m_fip->getWidth() / 2 * m_zoom)) / m_zoom);
		if (rc.bottom - rc.top < m_fip->getHeight() * m_zoom + MARGIN * 2)
			lp.y = static_cast<int>((dy - MARGIN + m_nVScrollPos) / m_zoom);
		else
			lp.y = static_cast<int>((dy - (rc.bottom / 2 - m_fip->getHeight() / 2 * m_zoom)) / m_zoom);
		return lp;
	}

	bool IsFocused() const
	{
		return m_hWnd == GetFocus();
	}

	void ScrollTo(int x, int y, bool force = false)
	{
		SCROLLINFO sih = {0}, siv = {0};
		sih.cbSize = sizeof SCROLLINFO;
		sih.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
		GetScrollInfo(m_hWnd, SB_HORZ, &sih);
		siv.cbSize = sizeof SCROLLINFO;
		siv.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
		GetScrollInfo(m_hWnd, SB_VERT, &siv);

		RECT rc;
		GetClientRect(m_hWnd, &rc);

		if (rc.right - rc.left < m_fip->getWidth() * m_zoom + MARGIN * 2)
		{
			if (force)
			{
				m_nHScrollPos = static_cast<int>(x * m_zoom + MARGIN - rc.right / 2);
			}
			else
			{
				if (x * m_zoom + MARGIN < m_nHScrollPos || m_nHScrollPos + rc.right < x * m_zoom + MARGIN)
					m_nHScrollPos = static_cast<int>(x * m_zoom + MARGIN - rc.right / 2);
			}
			if (m_nHScrollPos < 0)
				m_nHScrollPos = 0;
			else if (m_nHScrollPos > sih.nMax - static_cast<int>(sih.nPage))
				m_nHScrollPos = sih.nMax - sih.nPage;
		}
		if (rc.bottom - rc.top < m_fip->getHeight() * m_zoom + MARGIN * 2)
		{
			if (force)
			{
				m_nVScrollPos = static_cast<int>(y * m_zoom + MARGIN - rc.bottom / 2);
			}
			else
			{
				if (y * m_zoom + MARGIN < m_nVScrollPos || m_nVScrollPos + rc.bottom < y * m_zoom + MARGIN)
					m_nVScrollPos = static_cast<int>(y * m_zoom + MARGIN - rc.bottom / 2);
			}
			if (m_nVScrollPos < 0)
				m_nVScrollPos = 0;
			else if (m_nVScrollPos > siv.nMax - static_cast<int>(siv.nPage))
				m_nVScrollPos = siv.nMax - siv.nPage;
		}

		RECT rcClip = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
		ScrollWindow(m_hWnd, sih.nPos - m_nHScrollPos, siv.nPos - m_nVScrollPos, NULL, &rcClip);
		CalcScrollBarRange();
		InvalidateRect(m_hWnd, NULL, FALSE);
	}

	RGBQUAD GetBackColor() const
	{
		return m_backColor;
	}

	void SetBackColor(RGBQUAD backColor)
	{
		m_backColor = backColor;
		if (m_fip)
		{
			m_fip->setModified(true);
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}

	bool GetUseBackColor() const
	{
		return m_useBackColor;
	}

	void SetUseBackColor(bool useBackColor)
	{
		m_useBackColor = useBackColor;
		if (m_fip)
		{
			m_fip->setModified(true);
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}

	double GetZoom() const
	{
		return m_zoom;
	}

	void SetZoom(double zoom)
	{
		double oldZoom = m_zoom;
		m_zoom = zoom;
		if (m_zoom < 0.1)
			m_zoom = 0.1;
		m_nVScrollPos = static_cast<int>(m_nVScrollPos / oldZoom * m_zoom);
		m_nHScrollPos = static_cast<int>(m_nHScrollPos / oldZoom * m_zoom);
		if (m_fip)
		{
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			unsigned width  = static_cast<unsigned>(m_fip->getWidth()  * m_zoom) + MARGIN * 2; 
			unsigned height = static_cast<unsigned>(m_fip->getHeight() * m_zoom) + MARGIN * 2; 
			if (m_nHScrollPos > static_cast<int>(width  - rc.right))
				m_nHScrollPos = width  - rc.right;
			if (m_nHScrollPos < 0)
				m_nHScrollPos = 0;
			if (m_nVScrollPos > static_cast<int>(height - rc.bottom))
				m_nVScrollPos = height - rc.bottom;
			if (m_nVScrollPos < 0)
				m_nVScrollPos = 0;
			CalcScrollBarRange();
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}

	void Invalidate(bool erase = false)
	{
		InvalidateRect(m_hWnd, NULL, erase);
	}

	void SetImage(fipWinImage *pfip)
	{
		m_fip = pfip;
		CalcScrollBarRange();
	}

private:

	ATOM MyRegisterClass(HINSTANCE hInstance)
	{
		WNDCLASSEXW wcex = {0};
		if (!GetClassInfoEx(hInstance, L"WinImgWindowClass", &wcex))
		{
			wcex.cbSize         = sizeof(WNDCLASSEX); 
			wcex.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc	= (WNDPROC)WndProc;
			wcex.cbClsExtra		= 0;
			wcex.cbWndExtra		= 0;
			wcex.hInstance		= hInstance;
			wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground	= CreateSolidBrush(RGB(206, 215, 230));
			wcex.lpszClassName	= L"WinImgWindowClass";
		}
		return RegisterClassExW(&wcex);
	}

	void OnPaint()
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hWnd, &ps);
		if (m_fip)
		{
			RECT rcImg, rc;
			GetClientRect(m_hWnd, &rc);
			if (rc.right - rc.left > m_fip->getWidth() * m_zoom + MARGIN * 2)
			{
				rcImg.left  = static_cast<int>(((rc.right - rc.left) - m_fip->getWidth() * m_zoom) / 2); 
				rcImg.right = static_cast<int>(rcImg.left + m_fip->getWidth() * m_zoom); 
			}
			else
			{
				rcImg.left = -m_nHScrollPos + MARGIN;
				rcImg.right = static_cast<int>(m_fip->getWidth() * m_zoom + MARGIN - m_nHScrollPos);
			}
			if (rc.bottom - rc.top > m_fip->getHeight() * m_zoom + MARGIN * 2)
			{
				rcImg.top    = static_cast<int>(((rc.bottom - rc.top) - m_fip->getHeight() * m_zoom) / 2); 
				rcImg.bottom = static_cast<int>(rcImg.top + m_fip->getHeight() * m_zoom); 
			}
			else
			{
				rcImg.top    = -m_nVScrollPos + MARGIN;
				rcImg.bottom = static_cast<int>(m_fip->getHeight() * m_zoom + MARGIN - m_nVScrollPos);
			}

			if (m_fip->isValid())
				m_fip->drawEx(hdc, rcImg, false, m_useBackColor ? &m_backColor : NULL);
			
			if (GetFocus() == m_hWnd)
			{
				DrawFocusRect(hdc, &rc);
			}
			else
			{
				HPEN hPen = (HPEN)GetStockObject(WHITE_PEN);
				HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
				SelectObject(hdc, hOldPen);
				SelectObject(hdc, hOldBrush);
			}
		}
		EndPaint(m_hWnd, &ps);
	}

	void OnSize(UINT nType, int cx, int cy)
	{
		CalcScrollBarRange();
	}

	void OnHScroll(UINT nSBCode, UINT nPos)
	{
		SCROLLINFO si = {0};
		si.cbSize = sizeof SCROLLINFO;
		si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
		GetScrollInfo(m_hWnd, SB_HORZ, &si);
		switch (nSBCode) {
		case SB_LINEUP:
			--m_nHScrollPos;
			break;
		case SB_LINEDOWN:
			++m_nHScrollPos;
			break;
		case SB_PAGEUP:
			m_nHScrollPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			m_nHScrollPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			m_nHScrollPos = nPos;
			break;
		default: break;
		}
		if (m_nHScrollPos < 0)
			m_nHScrollPos = 0;
		if (m_nHScrollPos > si.nMax - static_cast<int>(si.nPage))
			m_nHScrollPos = si.nMax - si.nPage;
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		RECT rcClip = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
		ScrollWindow(m_hWnd, si.nPos - m_nHScrollPos, 0, NULL, &rcClip);
		CalcScrollBarRange();
	}

	void OnVScroll(UINT nSBCode, UINT nPos)
	{
		SCROLLINFO si = {0};
		si.cbSize = sizeof SCROLLINFO;
		si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
		GetScrollInfo(m_hWnd, SB_VERT, &si);
		switch (nSBCode) {
		case SB_LINEUP:
			--m_nVScrollPos;
			break;
		case SB_LINEDOWN:
			++m_nVScrollPos;
			break;
		case SB_PAGEUP:
			m_nVScrollPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			m_nVScrollPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			m_nVScrollPos = nPos;
			break;
		default: break;
		}
		if (m_nVScrollPos < 0)
			m_nVScrollPos = 0;
		if (m_nVScrollPos > si.nMax - static_cast<int>(si.nPage))
			m_nVScrollPos = si.nMax - si.nPage;
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		RECT rcClip = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
		ScrollWindow(m_hWnd, 0, si.nPos - m_nVScrollPos, NULL, &rcClip);
		CalcScrollBarRange();
	}

	void OnLButtonDown(UINT nFlags, int x, int y)
	{
		SetFocus();
	}

	void OnRButtonDown(UINT nFlags, int x, int y)
	{
		SetFocus();
	}

	void OnMouseWheel(UINT nFlags, short zDelta)
	{
		if (!(nFlags & MK_CONTROL))
		{ 
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			if (!(nFlags & MK_SHIFT))
			{
				if (rc.bottom - rc.top < m_fip->getHeight() * m_zoom + MARGIN * 2)
				{
					SCROLLINFO si = {0};
					si.cbSize = sizeof SCROLLINFO;
					si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
					GetScrollInfo(m_hWnd, SB_VERT, &si);
					m_nVScrollPos += - zDelta / (WHEEL_DELTA / 16);
					if (m_nVScrollPos < 0)
						m_nVScrollPos = 0;
					if (m_nVScrollPos > si.nMax - static_cast<int>(si.nPage))
						m_nVScrollPos = si.nMax - si.nPage;
					RECT rcClip = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
					ScrollWindow(m_hWnd, 0, si.nPos - m_nVScrollPos, NULL, &rcClip);
					CalcScrollBarRange();
				}
			}
			else
			{
				if (rc.right - rc.left < m_fip->getWidth() * m_zoom + MARGIN * 2)
				{
					SCROLLINFO si = {0};
					si.cbSize = sizeof SCROLLINFO;
					si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
					GetScrollInfo(m_hWnd, SB_HORZ, &si);
					m_nHScrollPos += - zDelta / (WHEEL_DELTA / 16);
					if (m_nHScrollPos < 0)
						m_nHScrollPos = 0;
					if (m_nHScrollPos > si.nMax - static_cast<int>(si.nPage))
						m_nHScrollPos = si.nMax - si.nPage;
					RECT rcClip = {rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1};
					ScrollWindow(m_hWnd, si.nPos - m_nHScrollPos, 0, NULL, &rcClip);
					CalcScrollBarRange();
				}
			}
		}
		else
		{
			if (zDelta > 0)
				SetZoom(m_zoom + 0.1);
			else
				SetZoom(m_zoom - 0.1);
		}
	}

	void OnSetFocus(HWND hwndOld)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}

	void OnKillFocus(HWND hwndNew)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}

	LRESULT OnWndMsg(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (iMsg)
		{
		case WM_PAINT:
			OnPaint();
			break;
		case WM_HSCROLL:
			OnHScroll((UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam));
			break;
		case WM_VSCROLL:
			OnVScroll((UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam));
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			break;
		case WM_RBUTTONDOWN:
			OnRButtonDown((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			break;
		case WM_MOUSEWHEEL:
			OnMouseWheel(GET_KEYSTATE_WPARAM(wParam), GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		case WM_SETFOCUS:
			OnSetFocus((HWND)wParam);
			break;
		case WM_KILLFOCUS:
			OnKillFocus((HWND)wParam);
			break;
		case WM_COMMAND:
			PostMessage(GetParent(m_hWnd), iMsg, wParam, lParam);
			break;
		case WM_SIZE:
			OnSize((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		default:
			return DefWindowProc(hwnd, iMsg, wParam, lParam);
		}
		return 0;
	}

	void CalcScrollBarRange()
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		SCROLLINFO si = {0};
		si.cbSize = sizeof(SCROLLINFO);
		if (m_fip)
		{
			unsigned width  = static_cast<unsigned>(m_fip->getWidth()  * m_zoom) + MARGIN * 2;
			unsigned height = static_cast<unsigned>(m_fip->getHeight() * m_zoom) + MARGIN * 2; 

			si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
			si.nMin = 0;
			si.nMax = height;
			si.nPage = rc.bottom;
			si.nPos = m_nVScrollPos;
			SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
			
			si.nMin = 0;
			si.nMax = width;
			si.nPage = rc.right;
			si.nPos = m_nHScrollPos;
			SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
		}
	}

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		if (iMsg == WM_NCCREATE)
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));
		CImgWindow *pImgWnd = reinterpret_cast<CImgWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		LRESULT lResult = pImgWnd->OnWndMsg(hwnd, iMsg, wParam, lParam);
		return lResult;
	}

	HWND m_hWnd;
	fipWinImage *m_fip;
	int m_nVScrollPos;
	int m_nHScrollPos;
	double m_zoom;
	bool m_useBackColor;
	RGBQUAD m_backColor;
};
