// H2O.h
//
// H2O makes OS handles appear like pointers to C++ objects.
// H2O2 implements traditional OS handle wrapper classes.
// Both H2O and H2O2 share a common set of decorator templates.
//
// Copyright (c) 2005-2010  David Nash (as of Win32++ v7.0.2)
// Copyright (c) 2011-2013  Jochen Neubeck
//
// Permission is hereby granted, free of charge, to
// any person obtaining a copy of this software and
// associated documentation files (the "Software"),
// to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice
// shall be included in all copies or substantial portions
// of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <malloc.h>
#include <assert.h>
#include <shlwapi.h>
#include <commctrl.h>
typedef struct _IMAGELIST { int unused; } IMAGELIST__;
#include <tchar.h>

#define maPanic FatalAppExitA
#define MAASSERT _ASSERT

#ifdef _DEBUG
#define ASSERT_MSG(cond, msg) \
	(cond) ? (void)0 : _RPT_BASE((_CRT_ASSERT, __FILE__, __LINE__, NULL, msg))
#else
#define ASSERT_MSG(cond, msg)
#endif

#include "MAUtil/String.h"

#ifdef UNICODE
#define MAUtil_Tchar(type) MAUtil::W##type
#else
#define MAUtil_Tchar(type) MAUtil::##type
#endif

typedef MAUtil_Tchar(String) String;

namespace H2O
{
	class Handle
	// Now Here Comes Her Majesty The H2O Universal Handle Class.
	// Instances are never constructed.
	// Instance pointers are obtained by casting OS handles.
	// The union members are protected by default.
	// Derived classes unprotect the one they use.
	{
	protected:
		union
		{
			HWND__ m_hWnd[1];
			HMENU__ m_hMenu[1];
			HFONT__ m_hFont[1];
			HBITMAP__ m_hBitmap[1];
			HPEN__ m_hPen[1];
			HBRUSH__ m_hBrush[1];
			int m_hObject[1];
			IMAGELIST__ m_hImageList[1];
			HDC__ m_hDC[1];
			// Members used by SysString
			OLECHAR B[20];
			CHAR A[20];
			TCHAR T[20];
			WCHAR W[20];
		};
	private:
		Handle(); // disallow construction
		void operator delete(void *); // disallow deletion
	};

	class HWindow;
	class HMenu;
	class HFont;
	class HBitmap;
	class HSurface;
	class HString;

	template<class Super>
	class Window : public Super
	{
	public:
		ATOM GetClassAtom()
		{
			assert(::IsWindow(m_hWnd));
			return ::GetClassWord(m_hWnd, GCW_ATOM);
		}
		ULONG_PTR GetClassLongPtrA(int nIndex)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetClassLongPtrA(m_hWnd, nIndex);
		}
		ULONG_PTR GetClassLongPtrW(int nIndex)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetClassLongPtrW(m_hWnd, nIndex);
		}
		int GetClassNameA(LPSTR lpClassName, int nMaxCount)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetClassNameA(m_hWnd, lpClassName, nMaxCount);
		}
		int GetClassNameW(LPWSTR lpClassName, int nMaxCount)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetClassNameW(m_hWnd, lpClassName, nMaxCount);
		}
		LONG_PTR GetWindowLongPtrA(int nIndex)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowLongPtrA(m_hWnd, nIndex);
		}
		LONG_PTR GetWindowLongPtrW(int nIndex)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowLongPtrW(m_hWnd, nIndex);
		}
		LONG_PTR SetWindowLongPtrA(int nIndex, LONG_PTR dwNewLong)
		{
			assert(::IsWindow(m_hWnd));
			return ::SetWindowLongPtrA(m_hWnd, nIndex, dwNewLong);
		}
		LONG_PTR SetWindowLongPtrW(int nIndex, LONG_PTR dwNewLong)
		{
			assert(::IsWindow(m_hWnd));
			return ::SetWindowLongPtrW(m_hWnd, nIndex, dwNewLong);
		}
		DWORD GetStyle()
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowLong(m_hWnd, GWL_STYLE);
		}
		DWORD GetExStyle()
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		}
		DWORD SetStyle(DWORD dw)
		// Returns set of altered style bits.
		{
			assert(::IsWindow(m_hWnd));
			return dw ^ ::SetWindowLong(m_hWnd, GWL_STYLE, dw);
		}
		DWORD SetExStyle(DWORD dw)
		// Returns set of altered exstyle bits.
		{
			assert(::IsWindow(m_hWnd));
			return dw ^ ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dw);
		}
		int GetScrollPos(int nBar)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetScrollPos(m_hWnd, nBar);
		}
		int SetScrollPos(int nBar, int nPos, BOOL bRedraw = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			return ::SetScrollPos(m_hWnd, nBar, nPos, bRedraw);
		}
		int GetScrollInfo(int nBar, SCROLLINFO *psi)
		{
			assert(::IsWindow(m_hWnd));
			assert(psi->cbSize == sizeof *psi);
			return ::GetScrollInfo(m_hWnd, nBar, psi);
		}
		int SetScrollInfo(int nBar, const SCROLLINFO *psi, BOOL bRedraw = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			assert(psi->cbSize == sizeof *psi);
			return ::SetScrollInfo(m_hWnd, nBar, psi, bRedraw);
		}
		int GetDlgCtrlID()
		{
			assert(::IsWindow(m_hWnd));
			return ::GetDlgCtrlID(m_hWnd);
		}
		int SetDlgCtrlID(int id)
		{
			assert(::IsWindow(m_hWnd));
			return static_cast<int>(::SetWindowLongPtr(m_hWnd, GWLP_ID, id));
		}
		BOOL Invalidate(BOOL bErase = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			return ::InvalidateRect(m_hWnd, NULL, bErase);
		}
		BOOL InvalidateRect(LPCRECT lprc, BOOL bErase = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			return ::InvalidateRect(m_hWnd, lprc, bErase);
		}
		BOOL UpdateWindow()
		{
			assert(::IsWindow(m_hWnd));
			return ::UpdateWindow(m_hWnd);
		}
		BOOL CreateCaret(HBITMAP hBitmap, int nWidth, int nHeight)
		{
			assert(::IsWindow(m_hWnd));
			return ::CreateCaret(m_hWnd, hBitmap, nWidth, nHeight);
		}
		BOOL ShowCaret()
		{
			assert(::IsWindow(m_hWnd));
			return ::ShowCaret(m_hWnd);
		}
		BOOL HideCaret()
		{
			assert(::IsWindow(m_hWnd));
			return ::HideCaret(m_hWnd);
		}
		BOOL ScrollWindow(int dx, int dy, const RECT *prcArea = NULL, const RECT *prcClip = NULL)
		{
			assert(::IsWindow(m_hWnd));
			return ::ScrollWindow(m_hWnd, dx, dy, prcArea, prcClip);
		}
		BOOL CloseWindow()
		{
			assert(::IsWindow(m_hWnd));
			return ::CloseWindow(m_hWnd);
		}
		BOOL DestroyWindow()
		{
			assert(::IsWindow(m_hWnd));
			return ::DestroyWindow(m_hWnd);
		}
		BOOL EndDialog(INT_PTR nResult)
		{
			assert(::IsWindow(m_hWnd));
			return ::EndDialog(m_hWnd, nResult);
		}
		BOOL IsDialogMessage(MSG *pMsg)
		{
			assert(::IsWindow(m_hWnd));
			return ::IsDialogMessage(m_hWnd, pMsg);
		}
		BOOL OpenClipboard()
		{
			assert(::IsWindow(m_hWnd));
			return ::OpenClipboard(m_hWnd);
		}
		UINT GetDlgCode(UINT uKey = 0, MSG *pMsg = NULL)
		{
			assert(::IsWindow(m_hWnd));
			return static_cast<UINT>(::SendMessage(m_hWnd, WM_GETDLGCODE, uKey, reinterpret_cast<LPARAM>(pMsg)));
		}
		HFont *GetFont()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HFont *>(::SendMessage(m_hWnd, WM_GETFONT, 0, 0));
		}
		void SetFont(HFont *pFont, BOOL bRedraw = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(pFont->m_hFont), bRedraw);
		}
		HICON GetIcon(int iImage = ICON_BIG)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HICON>(::SendMessage(m_hWnd, WM_GETICON, iImage, 0));
		}
		HICON SetIcon(HICON hIcon, int iImage = ICON_BIG)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HICON>(::SendMessage(m_hWnd, WM_SETICON, iImage, reinterpret_cast<LPARAM>(hIcon)));
		}
		BOOL FlashWindow(BOOL bInvert)
		{
			assert(::IsWindow(m_hWnd));
			return ::FlashWindow(m_hWnd, bInvert);
		}
		BOOL FlashWindowEx(FLASHWINFO *pfwi)
		{
			assert(::IsWindow(m_hWnd));
			assert(pwp->cbSize == sizeof *pfwi);
			return ::FlashWindowEx(m_hWnd, pfwi);
		}
		HWindow *FindWindowEx(HWindow *pwndChildAfter, LPCTSTR lpszClass, LPCTSTR lpszWindow = NULL)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::FindWindowEx(m_hWnd, pwndChildAfter->m_hWnd, lpszClass, lpszWindow));
		}
		HWindow *ChildWindowFromPoint(POINT pt)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::ChildWindowFromPoint(m_hWnd, pt));
		}
		HWindow *ChildWindowFromPoint(POINT pt, UINT nFlags)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::ChildWindowFromPointEx(m_hWnd, pt, nFlags));
		}
		HWindow *GetWindow(UINT nDirection)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::GetWindow(m_hWnd, nDirection));
		}
		HWindow *GetTopWindow()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::GetTopWindow(m_hWnd));
		}
		HWindow *GetLastActivePopup()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::GetLastActivePopup(m_hWnd));
		}
		HWindow *GetNextDlgTabItem(HWindow *pCtl, BOOL bPrevious = FALSE)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::GetNextDlgTabItem(m_hWnd, pCtl->m_hWnd, bPrevious));
		}
		HWindow *GetNextDlgGroupItem(HWindow *pCtl, BOOL bPrevious = FALSE)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::GetNextDlgGroupItem(m_hWnd, pCtl->m_hWnd, bPrevious));
		}
		HWindow *GetParent()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::GetParent(m_hWnd));
		}
		HWindow *SetParent(HWindow *pWndNewParent)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::SetParent(m_hWnd, pWndNewParent->m_hWnd));
		}
		HWindow *SetFocus()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::SetFocus(m_hWnd));
		}
		HWindow *SetCapture()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::SetCapture(m_hWnd));
		}
		HWindow *SetActiveWindow()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::SetActiveWindow(m_hWnd));
		}
		BOOL SetForegroundWindow()
		{
			assert(::IsWindow(m_hWnd));
			return ::SetForegroundWindow(m_hWnd);
		}
		BOOL GetWindowPlacement(WINDOWPLACEMENT *pwp)
		{
			assert(::IsWindow(m_hWnd));
			assert(pwp->length == sizeof *pwp);
			return ::GetWindowPlacement(m_hWnd, pwp);
		}
		BOOL SetWindowPlacement(const WINDOWPLACEMENT *pwp)
		{
			assert(::IsWindow(m_hWnd));
			assert(pwp->length == sizeof *pwp);
			return ::SetWindowPlacement(m_hWnd, pwp);
		}
		BOOL GetWindowRect(RECT *prc)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowRect(m_hWnd, prc);
		}
		BOOL GetClientRect(RECT *prc)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetClientRect(m_hWnd, prc);
		}
		BOOL ScreenToClient(POINT *ppt)
		{
			assert(::IsWindow(m_hWnd));
			return ::ScreenToClient(m_hWnd, ppt);
		}
		BOOL ClientToScreen(POINT *ppt)
		{
			assert(::IsWindow(m_hWnd));
			return ::ClientToScreen(m_hWnd, ppt);
		}
		BOOL ScreenToClient(RECT *prc)
		{
			assert(::IsWindow(m_hWnd));
			return ::MapWindowPoints(NULL, m_hWnd, reinterpret_cast<POINT *>(prc), 2);
		}
		BOOL ClientToScreen(RECT *prc)
		{
			assert(::IsWindow(m_hWnd));
			return ::MapWindowPoints(m_hWnd, NULL, reinterpret_cast<POINT *>(prc), 2);
		}
		BOOL MapWindowPoints(HWindow *pWndTo, LPPOINT lpPoints, UINT cPoints)
		{
			assert(::IsWindow(m_hWnd));
			return ::MapWindowPoints(m_hWnd, pWndTo->m_hWnd, lpPoints, cPoints);
		}
		BOOL SetWindowPos(HWindow *pWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags)
		{
			assert(::IsWindow(m_hWnd));
			return ::SetWindowPos(m_hWnd, pWndInsertAfter->m_hWnd, x, y, cx, cy, uFlags);
		}
		BOOL MoveWindow(int x, int y, int cx, int cy, BOOL bRedraw = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			return ::MoveWindow(m_hWnd, x, y, cx, cy, bRedraw);
		}
		BOOL BringWindowToTop()
		{
			assert(::IsWindow(m_hWnd));
			return ::BringWindowToTop(m_hWnd);
		}
		BOOL DrawMenuBar()
		{
			assert(::IsWindow(m_hWnd));
			return ::DrawMenuBar(m_hWnd);
		}
		BOOL RedrawWindow(LPCRECT lprcUpdate = NULL, HRGN hrgnUpdate = NULL,
			UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE)
		{
			assert(::IsWindow(m_hWnd));
			return ::RedrawWindow(m_hWnd, lprcUpdate, hrgnUpdate, flags);
		}
		void SetRedraw(BOOL bRedraw)
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_SETREDRAW, bRedraw, 0);
		}
		UINT_PTR SetTimer(UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc)
		{
			assert(::IsWindow(m_hWnd));
			return ::SetTimer(m_hWnd, nIDEvent, uElapse, lpTimerFunc);
		}
		BOOL KillTimer(UINT_PTR nIDEvent)
		{
			assert(::IsWindow(m_hWnd));
			return ::KillTimer(m_hWnd, nIDEvent);
		}
		BOOL IsWindow()
		{
			assert(::IsWindow(m_hWnd));
			return ::IsWindow(m_hWnd);
		}
		BOOL IsWindowVisible()
		{
			assert(::IsWindow(m_hWnd));
			return ::IsWindowVisible(m_hWnd);
		}
		BOOL IsWindowEnabled()
		{
			assert(::IsWindow(m_hWnd));
			return ::IsWindowEnabled(m_hWnd);
		}
		BOOL IsWindowUnicode()
		{
			assert(::IsWindow(m_hWnd));
			return ::IsWindowUnicode(m_hWnd);
		}
		BOOL IsIconic()
		{
			assert(::IsWindow(m_hWnd));
			return ::IsIconic(m_hWnd);
		}
		BOOL IsChild(HWindow *pWnd)
		{
			assert(::IsWindow(m_hWnd));
			return ::IsChild(m_hWnd, pWnd->m_hWnd);
		}
		BOOL ShowWindow(int nCmdShow)
		{
			assert(::IsWindow(m_hWnd));
			return ::ShowWindow(m_hWnd, nCmdShow);
		}
		BOOL EnableWindow(BOOL fEnable = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			return ::EnableWindow(m_hWnd, fEnable);
		}
		BOOL ShowScrollBar(int wBar, BOOL bShow)
		{
			assert(::IsWindow(m_hWnd));
			return ::ShowScrollBar(m_hWnd, wBar, bShow);
		}
		LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0)
		{
			assert(::IsWindow(m_hWnd));
			return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
		}
		BOOL PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0)
		{
			assert(::IsWindow(m_hWnd));
			return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
		}
		LRESULT SendDlgItemMessage(int id, UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0)
		{
			assert(::IsWindow(m_hWnd));
			return ::SendDlgItemMessage(m_hWnd, id, uMsg, wParam, lParam);
		}
		HWindow *GetDlgItem(int nIDDlgItem)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HWindow *>(::GetDlgItem(m_hWnd, nIDDlgItem));
		}
		BOOL CheckDlgButton(int nIDButton, UINT uCheck)
		{
			assert(::IsWindow(m_hWnd));
			return ::CheckDlgButton(m_hWnd, nIDButton, uCheck);
		}
		BOOL CheckRadioButton(int nIDFirstButton, int nIDLastButton, int nIDCheckButton)
		{
			assert(::IsWindow(m_hWnd));
			return ::CheckRadioButton(m_hWnd, nIDFirstButton, nIDLastButton, nIDCheckButton);
		}
		UINT IsDlgButtonChecked(int nIDButton)
		{
			assert(::IsWindow(m_hWnd));
			return ::IsDlgButtonChecked(m_hWnd, nIDButton);
		}
		int GetWindowTextLength()
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowTextLength(m_hWnd);
		}
		UINT GetWindowTextA(LPSTR lpString, int nMaxCount)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowTextA(m_hWnd, lpString, nMaxCount);
		}
		UINT GetWindowTextW(LPWSTR lpString, int nMaxCount)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetWindowTextW(m_hWnd, lpString, nMaxCount);
		}
		UINT GetWindowText(String &s)
		{
			assert(::IsWindow(m_hWnd));
			int size = ::GetWindowTextLength(m_hWnd);
			s.resize(size);
			return ::GetWindowText(m_hWnd, s.pointer(), size + 1);
		}
		UINT GetDlgItemTextA(int nIDDlgItem, LPSTR lpString, int nMaxCount)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetDlgItemTextA(m_hWnd, nIDDlgItem, lpString, nMaxCount);
		}
		UINT GetDlgItemTextW(int nIDDlgItem, LPWSTR lpString, int nMaxCount)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetDlgItemTextW(m_hWnd, nIDDlgItem, lpString, nMaxCount);
		}
		UINT GetDlgItemText(int id, String &s)
		{
			assert(::IsWindow(m_hWnd));
			int size = static_cast<int>(
				SendDlgItemMessage(id, WM_GETTEXTLENGTH, 0, 0));
			s.resize(size);
			return GetDlgItemText(id, s.pointer(), size + 1);
		}
		void SetWindowTextA(LPCSTR lpString)
		{
			assert(::IsWindow(m_hWnd));
			::SetWindowTextA(m_hWnd, lpString);
		}
		void SetWindowTextW(LPCWSTR lpString)
		{
			assert(::IsWindow(m_hWnd));
			::SetWindowTextW(m_hWnd, lpString);
		}
		void SetDlgItemTextA(int nIDDlgItem, LPCSTR lpString)
		{
			assert(::IsWindow(m_hWnd));
			::SetDlgItemTextA(m_hWnd, nIDDlgItem, lpString);
		}
		void SetDlgItemTextW(int nIDDlgItem, LPCWSTR lpString)
		{
			assert(::IsWindow(m_hWnd));
			::SetDlgItemTextW(m_hWnd, nIDDlgItem, lpString);
		}
		UINT GetDlgItemInt(int nIDDlgItem, BOOL *lpTranslated = NULL, BOOL bSigned = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetDlgItemInt(m_hWnd, nIDDlgItem, lpTranslated, bSigned);
		}
		BOOL SetDlgItemInt(int nIDDlgItem, UINT uValue, BOOL bSigned = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			return ::SetDlgItemInt(m_hWnd, nIDDlgItem, uValue, bSigned);
		}
		HMenu *GetMenu()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HMenu *>(::GetMenu(m_hWnd));
		}
		BOOL SetMenu(HMENU hMenu)
		{
			assert(::IsWindow(m_hWnd));
			return ::SetMenu(m_hWnd, hMenu);
		}
		HMenu *GetSystemMenu(BOOL bRevert)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HMenu *>(::GetSystemMenu(m_hWnd, bRevert));
		}
		HSurface *GetDC()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HSurface *>(::GetDC(m_hWnd));
		}
		HSurface *GetWindowDC()
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HSurface *>(::GetWindowDC(m_hWnd));
		}
		int ReleaseDC(HSurface *pDC)
		{
			assert(::IsWindow(m_hWnd));
			return ::ReleaseDC(m_hWnd, pDC->m_hDC);
		}
		HSurface *BeginPaint(PAINTSTRUCT *pps)
		{
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HSurface *>(::BeginPaint(m_hWnd, pps));
		}
		BOOL EndPaint(const PAINTSTRUCT *pps)
		{
			assert(::IsWindow(m_hWnd));
			return ::EndPaint(m_hWnd, pps);
		}
		BOOL GetUpdateRect(LPRECT lpRect, BOOL bErase)
		{
			assert(::IsWindow(m_hWnd));
			return ::GetUpdateRect(m_hWnd, lpRect, bErase);
		}
		int MessageBoxA(LPCSTR text, LPCSTR caption, UINT style)
		{
			assert(::IsWindow(m_hWnd));
			return ::MessageBoxA(m_hWnd, text, caption, style);
		}
		int MessageBoxW(LPCWSTR text, LPCWSTR caption, UINT style)
		{
			assert(::IsWindow(m_hWnd));
			return ::MessageBoxW(m_hWnd, text, caption, style);
		}
	};

	class HWindow : public Window<Handle>
	{
	public:
		using Window<Handle>::m_hWnd;
		static HWindow *CreateEx(DWORD exStyle,
			LPCTSTR className, LPCTSTR windowName, DWORD style,
			int x, int y, int cx, int cy, HWindow *parent, UINT id,
			HINSTANCE hinst = NULL, void *param = NULL)
		{
			HWND hWnd = ::CreateWindowEx(exStyle, className, windowName, style,
				x, y, cx, cy, parent->m_hWnd, reinterpret_cast<HMENU>(id), hinst, param);
			return reinterpret_cast<HWindow *>(hWnd);
		}
		static HWindow *GetConsoleWindow()
		{
			return reinterpret_cast<HWindow *>(::GetConsoleWindow());
		}
		static HWindow *GetDesktopWindow()
		{
			return reinterpret_cast<HWindow *>(::GetDesktopWindow());
		}
		static HWindow *GetActiveWindow()
		{
			return reinterpret_cast<HWindow *>(::GetActiveWindow());
		}
		static HWindow *GetForegroundWindow()
		{
			return reinterpret_cast<HWindow *>(::GetForegroundWindow());
		}
		static HWindow *GetFocus()
		{
			return reinterpret_cast<HWindow *>(::GetFocus());
		}
		static HWindow *GetCapture()
		{
			return reinterpret_cast<HWindow *>(::GetCapture());
		}
		static HWindow *FindWindow(LPCTSTR lpszClass, LPCTSTR lpszWindow = NULL)
		{
			return reinterpret_cast<HWindow *>(::FindWindow(lpszClass, lpszWindow));
		}
		static HWindow *WindowFromPoint(POINT pt)
		{
			return reinterpret_cast<HWindow *>(::WindowFromPoint(pt));
		}
	};

	template<class Super>
	class Menu : public Super
	{
	public:
		BOOL DestroyMenu()
		{
			assert(::IsMenu(m_hMenu));
			return ::DestroyMenu(m_hMenu);
		}
		BOOL DeleteMenu(UINT index, UINT flags = MF_BYCOMMAND)
		{
			assert(::IsMenu(m_hMenu));
			return ::DeleteMenu(m_hMenu, index, flags);
		}
		BOOL RemoveMenu(UINT index, UINT flags = MF_BYCOMMAND)
		{
			assert(::IsMenu(m_hMenu));
			return ::RemoveMenu(m_hMenu, index, flags);
		}
		BOOL TrackPopupMenu(UINT nFlags, int x, int y, HWindow *pWnd, LPCRECT lpRect = NULL)
		{
			assert(::IsMenu(m_hMenu));
			return ::TrackPopupMenu(m_hMenu, nFlags, x, y, NULL, pWnd->m_hWnd, lpRect);
		}
		BOOL TrackPopupMenuEx(UINT nFlags, int x, int y, HWindow *pWnd, TPMPARAMS *pParams = NULL)
		{
			assert(::IsMenu(m_hMenu));
			return ::TrackPopupMenuEx(m_hMenu, nFlags, x, y, pWnd->m_hWnd, pParams);
		}
		BOOL GetMenuInfo(LPMENUINFO pmi)
		{
			assert(pmi->cbSize == sizeof *pmi);
			assert(::IsMenu(m_hMenu));
			return ::GetMenuInfo(m_hMenu, pmi);
		}
		BOOL SetMenuInfo(LPCMENUINFO pmi)
		{
			assert(pmi->cbSize == sizeof *pmi);
			assert(::IsMenu(m_hMenu));
			return ::SetMenuInfo(m_hMenu, pmi);
		}
		BOOL GetMenuItemInfo(UINT id, BOOL byPos, LPMENUITEMINFO pmii)
		{
			assert(pmii->cbSize == sizeof *pmii);
			assert(::IsMenu(m_hMenu));
			return ::GetMenuItemInfo(m_hMenu, id, byPos, pmii);
		}
		BOOL SetMenuItemInfo(UINT id, BOOL byPos, LPCMENUITEMINFO pmii)
		{
			assert(pmii->cbSize == sizeof *pmii);
			assert(::IsMenu(m_hMenu));
			return ::SetMenuItemInfo(m_hMenu, id, byPos, pmii);
		}
		BOOL InsertMenuItem(UINT id, BOOL byPos, LPMENUITEMINFO pmii)
		{
			assert(pmii->cbSize == sizeof *pmii);
			assert(::IsMenu(m_hMenu));
			return ::InsertMenuItem(m_hMenu, id, byPos, pmii);
		}
		BOOL AppendMenu(UINT flags, UINT_PTR id = 0, LPCTSTR text = NULL)
		{
			assert(::IsMenu(m_hMenu));
			return ::AppendMenu(m_hMenu, flags, id, text);
		}
		BOOL ModifyMenu(UINT index, UINT flags, UINT_PTR id = 0, LPCTSTR text = NULL)
		{
			assert(::IsMenu(m_hMenu));
			return ::ModifyMenu(m_hMenu, index, flags, id, text);
		}
		BOOL InsertMenu(UINT index, UINT flags, UINT_PTR id = 0, LPCTSTR text = NULL)
		{
			assert(::IsMenu(m_hMenu));
			return ::InsertMenu(m_hMenu, index, flags, id, text);
		}
		BOOL SetMenuItemBitmaps(UINT id, UINT flags, HBITMAP hbmUnchecked, HBITMAP hbmChecked)
		{
			assert(::IsMenu(m_hMenu));
			return ::SetMenuItemBitmaps(m_hMenu, id, flags, hbmUnchecked, hbmChecked);
		}
		UINT CheckMenuItem(UINT id, UINT state)
		{
			assert(::IsMenu(m_hMenu));
			return ::CheckMenuItem(m_hMenu, id, state);
		}
		BOOL CheckMenuRadioItem(UINT idFirst, UINT idLast, UINT id, UINT flags = MF_BYCOMMAND)
		{
			assert(::IsMenu(m_hMenu));
			return ::CheckMenuRadioItem(m_hMenu, idFirst, idLast, id, flags);
		}
		UINT EnableMenuItem(UINT id, UINT state)
		{
			assert(::IsMenu(m_hMenu));
			return ::EnableMenuItem(m_hMenu, id, state);
		}
		UINT GetMenuItemCount()
		{
			assert(::IsMenu(m_hMenu));
			return ::GetMenuItemCount(m_hMenu);
		}
		UINT GetMenuItemID(int index)
		{
			assert(::IsMenu(m_hMenu));
			return ::GetMenuItemID(m_hMenu, index);
		}
		UINT GetMenuState(UINT id, UINT flags = MF_BYCOMMAND)
		{
			assert(::IsMenu(m_hMenu));
			return ::GetMenuState(m_hMenu, id, flags);
		}
		int GetMenuStringA(UINT id, LPSTR text, int limit, UINT flags = MF_BYCOMMAND)
		{
			assert(::IsMenu(m_hMenu));
			return ::GetMenuStringA(m_hMenu, id, text, limit, flags);
		}
		int GetMenuStringW(UINT id, LPWSTR text, int limit, UINT flags = MF_BYCOMMAND)
		{
			assert(::IsMenu(m_hMenu));
			return ::GetMenuStringW(m_hMenu, id, text, limit, flags);
		}
		BOOL SetMenuDefaultItem(UINT id, BOOL byPos = FALSE)
		{
			assert(::IsMenu(m_hMenu));
			return ::SetMenuDefaultItem(m_hMenu, id, byPos);
		}
		UINT GetMenuDefaultItem(BOOL byPos = FALSE, UINT gmdiFlags = 0)
		{
			assert(::IsMenu(m_hMenu));
			return ::GetMenuDefaultItem(m_hMenu, byPos, gmdiFlags);
		}
		HMenu *GetSubMenu(int index)
		{
			assert(::IsMenu(m_hMenu));
			return reinterpret_cast<HMenu *>(::GetSubMenu(m_hMenu, index));
		}
	};

	class HMenu : public Menu<Handle>
	{
	public:
		using Menu<Handle>::m_hMenu;
		// Creation
		static HMenu *CreateMenu()
		{
			return reinterpret_cast<HMenu *>(::CreateMenu());
		}
		static HMenu *CreatePopupMenu()
		{
			return reinterpret_cast<HMenu *>(::CreatePopupMenu());
		}
		static HMenu *LoadMenu(HINSTANCE hinst, LPCTSTR name)
		{
			return reinterpret_cast<HMenu *>(::LoadMenu(hinst, name));
		}
	};

	class HGdiObj : public Handle
	{
	public:
		using Handle::m_hObject;
		static HGdiObj *GetStockObject(int fnObject)
		{
			return reinterpret_cast<HGdiObj *>(::GetStockObject(fnObject));
		}
		BOOL DeleteObject()
		{
			return ::DeleteObject(m_hObject);
		}
	};

	template<class Super>
	class Font : public Super
	{
	public:
		int GetLogFont(LOGFONT *plf)
		{
			return ::GetObject(m_hFont, sizeof *plf, plf);
		}
	};

	class HFont : public Font<HGdiObj>
	{
	public:
		using Font<HGdiObj>::m_hFont;
		// Creation
		static HFont *Create(
			int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight,
			DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision,
			DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCTSTR pszFaceName)
		{
			return reinterpret_cast<HFont *>(CreateFont(
				cHeight, cWidth, cEscapement, cOrientation, cWeight,
				bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision,
				iClipPrecision, iQuality, iPitchAndFamily, pszFaceName));
		}
		static HFont *CreateIndirect(const LOGFONT *plf)
		{
			return reinterpret_cast<HFont *>(::CreateFontIndirect(plf));
		}
	};

	template<class Super>
	class Bitmap : public Super
	{
	public:
		int GetBitmap(BITMAP *pbm)
		{
			return ::GetObject(m_hBitmap, sizeof *pbm, pbm);
		}
	};

	class HBitmap : public Bitmap<HGdiObj>
	{
	public:
		using Bitmap<HGdiObj>::m_hBitmap;
	};

	template<class Super>
	class Brush : public Super
	{
	public:
		int GetLogBrush(LOGBRUSH *plb)
		{
			return ::GetObject(m_hBrush, sizeof *plb, plb);
		}
	};

	class HBrush : public Brush<HGdiObj>
	{
	public:
		using Brush<HGdiObj>::m_hBrush;
		static HBrush *CreateSolidBrush(COLORREF crColor)
		{
			return reinterpret_cast<HBrush *>(::CreateSolidBrush(crColor));
		}
	};

	template<class Super>
	class Pen : public Super
	{
	public:
		int GetLogPen(LOGPEN *plp)
		{
			return ::GetObject(m_hPen, sizeof *plp, plp);
		}
		int GetExtLogPen(EXTLOGPEN *pelp)
		{
			return ::GetObject(m_hPen, sizeof *pelp, pelp);
		}
	};

	class HPen : public Pen<HGdiObj>
	{
	public:
		using Pen<HGdiObj>::m_hPen;
		static HPen *Create(int fnPenStyle, int nWidth, COLORREF crColor)
		{
			return reinterpret_cast<HPen *>(::CreatePen(fnPenStyle, nWidth, crColor));
		}
	};

	template<class Super>
	class ImageList : public Super
	{
	public:
		int GetImageCount()
		{
			return ::ImageList_GetImageCount(m_hImageList);
		}
		int Add(HBITMAP hbmImage, HBITMAP hbmMask)
		{
			return ::ImageList_Add(m_hImageList, hbmImage, hbmMask);
		}
		int AddMasked(HBITMAP hbmImage, COLORREF mask)
		{
			return ::ImageList_AddMasked(m_hImageList, hbmImage, mask);
		}
		int Add(HICON hicon)
		{
			return ::ImageList_AddIcon(m_hImageList, hicon);
		}
		int Replace(int i, HICON hicon)
		{
			return ::ImageList_ReplaceIcon(m_hImageList, i, hicon);
		}
		BOOL Draw(int i, HDC hdc, int x, int y, UINT style)
		{
			return ::ImageList_Draw(m_hImageList, i, hdc, x, y, style);
		}
		BOOL Destroy()
		{
			return ::ImageList_Destroy(m_hImageList);
		}
	};

	class HImageList : public ImageList<Handle>
	{
	public:
		using ImageList<Handle>::m_hImageList;
		// Creation
		static HImageList *LoadImage(HINSTANCE hinst, LPCTSTR name, int cx, int grow, COLORREF mask, UINT type, UINT flags)
		{
			return reinterpret_cast<HImageList *>(::ImageList_LoadImage(hinst, name, cx, grow, mask, type, flags));
		}
		static HImageList *Create(int cx, int cy, UINT flags, int cInitial, int cGrow)
		{
			return reinterpret_cast<HImageList *>(::ImageList_Create(cx, cy, flags, cInitial, cGrow));
		}
	};

	template<class Super>
	class Surface : public Super
	{
	public:
		HGdiObj *SelectObject(HGdiObj *pGdiObj)
		{
			return reinterpret_cast<HGdiObj *>(::SelectObject(m_hDC, pGdiObj->m_hObject));
		}
		HGdiObj *SelectStockObject(int fnObject)
		{
			return SelectObject(HGdiObj::GetStockObject(fnObject));
		}
		HSurface *CreateCompatibleDC()
		{
			return reinterpret_cast<HSurface *>(::CreateCompatibleDC(m_hDC));
		}
		HBitmap *CreateCompatibleBitmap(int nWidth, int nHeight)
		{
			return reinterpret_cast<HBitmap *>(::CreateCompatibleBitmap(m_hDC, nWidth, nHeight));
		}
		HBitmap *CreateDIBSection(const BITMAPINFO *pbmi, UINT iUsage, void **ppvBits, HANDLE hSection = NULL, DWORD dwOffset = 0)
		{
			return reinterpret_cast<HBitmap *>(::CreateDIBSection(m_hDC, pbmi, iUsage, ppvBits, hSection, dwOffset));
		}
		COLORREF GetTextColor()
		{
			return ::GetTextColor(m_hDC);
		}
		COLORREF SetTextColor(COLORREF cr)
		{
			return ::SetTextColor(m_hDC, cr);
		}
		COLORREF GetBkColor()
		{
			return ::GetBkColor(m_hDC);
		}
		COLORREF SetBkColor(COLORREF cr)
		{
			return ::SetBkColor(m_hDC, cr);
		}
		int SetBkMode(int mode)
		{
			return ::SetBkMode(m_hDC, mode);
		}
		UINT SetTextAlign(UINT mode)
		{
			return ::SetTextAlign(m_hDC, mode);
		}
		BOOL TextOut(int x, int y, LPCTSTR pch, int cch)
		{
			return ::TextOut(m_hDC, x, y, pch, cch);
		}
		BOOL ExtTextOut(int x, int y, UINT options, const RECT *prc,
			LPCTSTR pch, int cch, const INT *pdx = NULL)
		{
			return ::ExtTextOut(m_hDC, x, y, options, prc, pch, cch, pdx);
		}
		int DrawTextA(LPCSTR pch, int cch, RECT *prc, UINT format)
		{
			return ::DrawTextA(m_hDC, pch, cch, prc, format);
		}
		int DrawTextW(LPCWSTR pch, int cch, RECT *prc, UINT format)
		{
			return ::DrawTextW(m_hDC, pch, cch, prc, format);
		}
		int DrawText(const String &s, RECT *prc, UINT format)
		{
			return ::DrawText(m_hDC, s.c_str(), s.length(), prc, format);
		}
		BOOL PathCompactPath(LPTSTR path, UINT dx)
		{
			return ::PathCompactPath(m_hDC, path, dx);
		}
		int FrameRect(const RECT *prc, HBrush *pbr)
		{
			return ::FrameRect(m_hDC, prc, pbr->m_hBrush);
		}
		int FillRect(const RECT *prc, HBrush *pbr)
		{
			return ::FillRect(m_hDC, prc, pbr->m_hBrush);
		}
		BOOL InvertRect(const RECT *prc)
		{
			return ::InvertRect(m_hDC, prc);
		}
		BOOL MoveTo(int x, int y, POINT *ppt = NULL)
		{
			return ::MoveToEx(m_hDC, x, y, ppt);
		}
		BOOL LineTo(int x, int y)
		{
			return ::LineTo(m_hDC, x, y);
		}
		BOOL Rectangle(int left, int top, int right, int bottom)
		{
			return ::Rectangle(m_hDC, left, top, right, bottom);
		}
		BOOL RoundRect(int left, int top, int right, int bottom, int width, int height)
		{
			return ::RoundRect(m_hDC, left, top, right, bottom, width, height);
		}
		int SetPolyFillMode(int mode)
		{
			return ::SetPolyFillMode(m_hDC, mode);
		}
		BOOL Polygon(const POINT *ppt, int count)
		{
			return ::Polygon(m_hDC, ppt, count);
		}
		BOOL Polyline(const POINT *ppt, int count)
		{
			return ::Polyline(m_hDC, ppt, count);
		}
		BOOL BitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HSurface *pdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
		{
			return ::BitBlt(m_hDC, nXDest, nYDest, nWidth, nHeight, pdcSrc->m_hDC, nXSrc, nYSrc, dwRop);
		}
		BOOL PatBlt(int nXLeft, int nYLeft, int nWidth, int nHeight, DWORD dwRop)
		{
			return ::PatBlt(m_hDC, nXLeft, nYLeft, nWidth, nHeight, dwRop);
		}
		int GetClipBox(RECT *prc)
		{
			return ::GetClipBox(m_hDC, prc);
		}
		BOOL GetTextExtent(LPCTSTR pch, int cch, SIZE *size)
		{
			return ::GetTextExtentPoint32(m_hDC, pch, cch, size);
		}
		BOOL GetTextExtentExPoint(LPCTSTR pch, int cch, int maxExtent, LPINT fit, LPINT dx, SIZE *size)
		{
			return ::GetTextExtentExPoint(m_hDC, pch, cch, maxExtent, fit, dx, size);
		}
		BOOL GetCharWidth(UINT firstChar, UINT lastChar, INT *buffer)
		{
			return ::GetCharWidth32(m_hDC, firstChar, lastChar, buffer);
		}
		BOOL GetTextMetricsA(TEXTMETRICA *ptm)
		{
			return ::GetTextMetricsA(m_hDC, ptm);
		}
		BOOL GetTextMetricsW(TEXTMETRICW *ptm)
		{
			return ::GetTextMetricsW(m_hDC, ptm);
		}
		UINT GetOutlineTextMetricsA(UINT cjCopy, OUTLINETEXTMETRICA *potm)
		{
			return ::GetOutlineTextMetricsA(m_hDC, cjCopy, potm);
		}
		UINT GetOutlineTextMetricsW(UINT cjCopy, OUTLINETEXTMETRICW *potm)
		{
			return ::GetOutlineTextMetricsW(m_hDC, cjCopy, potm);
		}
		int GetTextFaceA(int c, LPSTR name)
		{
			return ::GetTextFaceA(m_hDC, c, name);
		}
		int GetTextFaceW(int c, LPWSTR name)
		{
			return ::GetTextFaceW(m_hDC, c, name);
		}
		int IntersectClipRect(int left, int top, int right, int bottom)
		{
			return ::IntersectClipRect(m_hDC, left, top, right, bottom);
		}
		int ExcludeClipRect(int left, int top, int right, int bottom)
		{
			return ::ExcludeClipRect(m_hDC, left, top, right, bottom);
		}
		BOOL RectVisible(const RECT *prc)
		{
			return ::RectVisible(m_hDC, prc);
		}
		int GetDeviceCaps(int index)
		{
			return ::GetDeviceCaps(m_hDC, index);
		}
		int SaveDC()
		{
			return ::SaveDC(m_hDC);
		}
		BOOL RestoreDC(int state)
		{
			return ::RestoreDC(m_hDC, state);
		}
		BOOL DeleteDC()
		{
			return ::DeleteDC(m_hDC);
		}
		int ReleaseDC(HWND hWnd)
		{
			return ::ReleaseDC(hWnd, m_hDC);
		}
	};

	class HSurface : public Surface<Handle>
	{
	public:
		using Surface<Handle>::m_hDC;
		// Creation
		static HSurface *CreateDC(LPCTSTR driver, LPCTSTR device = NULL, const DEVMODE *devmode = NULL)
		{
			return reinterpret_cast<HSurface *>(::CreateDC(driver, device, NULL, devmode));
		}
	};

	template<class Super>
	class ListBox : public Super
	{
	public:
		int GetCount()
		// Returns the number of items in the list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETCOUNT, 0, 0);
		}

		int GetHorizontalExtent()
		// Returns the scrollable width, in pixels, of a list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETHORIZONTALEXTENT,	0, 0);
		}

		DWORD GetItemData(int nIndex)
		// Returns the value associated with the specified item.
		{
			assert(::IsWindow(m_hWnd));
			return (DWORD)::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0);
		}

		void* GetItemDataPtr(int nIndex)
		// Returns the value associated with the specified item.
		{
			assert(::IsWindow(m_hWnd));
			return (LPVOID)::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0);
		}

		int GetItemHeight(int nIndex)
		// Returns the height, in pixels, of an item in a list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETITEMHEIGHT, nIndex, 0L);
		}

		int GetItemRect(int nIndex, LPRECT lpRect)
		// Retrieves the client coordinates of the specified list box item.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETITEMRECT, nIndex, (LPARAM)lpRect);
		}

		LCID GetLocale()
		// Retrieves the locale of the list box. The high-order word contains the country/region code 
		//  and the low-order word contains the language identifier.
		{
			assert(::IsWindow(m_hWnd));
			return (LCID)::SendMessage(m_hWnd, LB_GETLOCALE, 0, 0);
		}

		int GetSel(int nIndex)
		// Returns the selection state of a list box item.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETSEL, nIndex, 0);
		}

		int GetText(int nIndex, LPTSTR lpszBuffer)
		// Retrieves the string associated with a specified item and the length of the string.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETTEXT, nIndex, (LPARAM)lpszBuffer);
		}

		int GetTextLen(int nIndex)
		// Returns the length, in characters, of the string associated with a specified item.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETTEXTLEN, nIndex, 0);
		}

		int GetText(int nIndex, String &s)
		{
			int size = GetTextLen(nIndex);
			s.resize(size);
			return GetText(nIndex, s.pointer());
		}

		int GetTopIndex()
		// Returns the index of the first visible item in a list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETTOPINDEX, 0, 0);
		}

		void SetColumnWidth(int cxWidth)
		// Sets the width, in pixels, of all columns in a list box.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, LB_SETCOLUMNWIDTH, cxWidth, 0);
		}

		void SetHorizontalExtent(int cxExtent)
		// Sets the scrollable width, in pixels, of a list box.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, LB_SETHORIZONTALEXTENT, cxExtent, 0);
		}

		int SetItemData(int nIndex, DWORD dwItemData)
		// Associates a value with a list box item.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
		}

		int SetItemDataPtr(int nIndex, void* pData)
		// Associates a value with a list box item.
		{
			assert(::IsWindow(m_hWnd));
			return SetItemData(nIndex, (DWORD)pData);
		}

		int SetItemHeight(int nIndex, UINT cyItemHeight)
		// Sets the height, in pixels, of an item or items in a list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
		}

		LCID SetLocale(LCID nNewLocale)
		// Sets the locale of a list box and returns the previous locale identifier.
		{
			assert(::IsWindow(m_hWnd));
			return (LCID)::SendMessage(m_hWnd, LB_SETLOCALE, (WPARAM)nNewLocale, 0);
		}

		BOOL SetTabStops(int nTabStops, LPINT rgTabStops)
		// Sets the tab stops to those specified in a specified array.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
		}

		void SetTabStops()
		// Sets default tab stops.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, LB_SETTABSTOPS, 0, 0);
		}

		BOOL SetTabStops(int cxEachStop)
		// Sets equidistant tab stops.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, 1, (LPARAM)&cxEachStop);
		}

		int SetTopIndex(int nIndex)
		// Scrolls the list box so the specified item is at the top of the visible range.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_SETTOPINDEX, nIndex, 0);
		}

		int GetCurSel()
		// Returns the index of the currently selected item.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETCURSEL, 0, 0);
		}

		int SetCurSel(int nSelect)
		// Selects a specified list box item.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_SETCURSEL, nSelect, 0);
		}

		int GetAnchorIndex()
		// Returns the index of the item that the mouse last selected.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETANCHORINDEX, 0, 0);
		}

		int GetCaretIndex()
		// Returns the index of the item that has the focus rectangle.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETCARETINDEX, 0, 0L);
		}

		int GetSelCount()
		// Returns the number of selected items in a multiple-selection list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETSELCOUNT, 0, 0);
		}

		int GetSelItems(int nMaxItems, LPINT rgIndex)
		// Creates an array of the indexes of all selected items in a multiple-selection list box 
		// and returns the total number of selected items.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_GETSELITEMS, nMaxItems, (LPARAM)rgIndex);
		}

		int SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
		// Selects a specified range of items in a list box.
		{
			assert(::IsWindow(m_hWnd));
			if (bSelect)
				return (int)::SendMessage(m_hWnd, LB_SELITEMRANGEEX, nFirstItem, nLastItem);
			else
				return (int)::SendMessage(m_hWnd, LB_SELITEMRANGEEX, nLastItem, nFirstItem);
		}

		void SetAnchorIndex(int nIndex)
		// Sets the item that the mouse last selected to a specified item.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, LB_SETANCHORINDEX, nIndex, 0);
		}

		int SetCaretIndex(int nIndex, BOOL bScroll)
		// Sets the focus rectangle to a specified list box item.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_SETCARETINDEX, nIndex, MAKELONG(bScroll, 0));
		}

		int SetSel(int nIndex, BOOL bSelect)
		// Selects an item in a multiple-selection list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_SETSEL, bSelect, nIndex);
		}

		int AddString(LPCTSTR lpszItem)
		// Adds a string to a list box and returns its index.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_ADDSTRING, 0, (LPARAM)lpszItem);
		}

		int DeleteString(UINT nIndex)
		// Removes a string from a list box and returns the number of strings remaining in the list.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_DELETESTRING, nIndex, 0);
		}

		int Dir(UINT attr, LPCTSTR lpszWildCard)
		// Adds a list of filenames to a list box and returns the index of the last filename added.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_DIR, attr, (LPARAM)lpszWildCard);
		}

		int FindString(int nStartAfter, LPCTSTR lpszItem)
		// Returns the index of the first string in the list box that begins with a specified string.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_FINDSTRING, nStartAfter, (LPARAM)lpszItem);
		}

		int FindStringExact(int nIndexStart, LPCTSTR lpszFind)
		// Returns the index of the string in the list box that is equal to a specified string.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
		}

		int InsertString(int nIndex, LPCTSTR lpszItem)
		// Inserts a string at a specified index in a list box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_INSERTSTRING, nIndex, (LPARAM)lpszItem);
		}

		void ResetContent()
		// Removes all items from a list box.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, LB_RESETCONTENT, 0, 0);
		}

		int SelectString(int nStartAfter, LPCTSTR lpszItem)
		// Selects the first string it finds that matches a specified prefix.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, LB_SELECTSTRING, nStartAfter, (LPARAM)lpszItem);
		}
	};

	class HListBox : public ListBox<HWindow>
	{
	public:
		static HListBox *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, WC_LISTBOX, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HListBox *>(pWnd);
		}
	};

	template<class Super>
	class ComboBox : public Super
	{
	public:
		int AddString(LPCTSTR lpszString)
		// Adds a string to the list box of a combo box. If the combo box does not 
		// have the CBS_SORT style, the string is added to the end of the list. 
		// Otherwise, the string is inserted into the list, and the list is sorted.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpszString);
		}

		void Clear()
		// Deletes the current selection, if any, from the combo box's edit control.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_CLEAR, 0, 0);
		}

		void Copy()
		// Copies the current selection to the clipboard in CF_TEXT format.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_COPY, 0, 0);
		}

		void Cut()
		// Deletes the current selection, if any, in the edit control and copies 
		// the deleted text to the clipboard in CF_TEXT format.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_CUT, 0, 0);
		}

		int DeleteString(int nIndex)
		// Deletes a string in the list box of a combo box. 
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_DELETESTRING, (WPARAM)nIndex, 0);
		}

		int Dir(UINT attr, LPCTSTR lpszWildCard)
		// Adds the names of directories and files that match a specified string 
		// and set of file attributes.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_DIR, (WPARAM)attr, (LPARAM)lpszWildCard);
		}

		int FindString(int nIndexStart, LPCTSTR lpszString)
		// Search the list box of a combo box for an item beginning with the 
		// characters in a specified string.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_FINDSTRING, (WPARAM)nIndexStart, (LPARAM)lpszString);
		}

		int FindStringExact(int nIndexStart, LPCTSTR lpszString)
		// Find the first list box string in a combo box that matches the string specified in lpszString. 
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_FINDSTRINGEXACT, (WPARAM)nIndexStart, (LPARAM)lpszString);
		}

		int GetCount()
		// Retrieves the number of items in the list box of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETCOUNT, 0,0);
		}

		int GetCurSel()
		// Retrieves the index of the currently selected item, if any, in the list box of the combo box. 
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETCURSEL, 0,0);
		}

		void GetDroppedControlRect(LPRECT prc)
		// Retrieves the screen coordinates of the combo box in its dropped-down state. 
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)prc);
		}

		BOOL GetDroppedState()
		// Determines whether the list box of the combo box is dropped down.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, CB_GETDROPPEDSTATE, 0, 0);
		}

		int GetDroppedWidth()
		// Retrieves the minimum allowable width, in pixels, of the list box of the combo box 
		// with the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETDROPPEDWIDTH, 0, 0);
		}

		DWORD GetEditSel()
		// Gets the starting and ending character positions of the current selection 
		// in the edit control of the combo box. 
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETEDITSEL, 0, 0);
		}

		BOOL GetExtendedUI()
		// Determines whether the combo box has the default user interface or the extended user interface.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, CB_GETEXTENDEDUI, 0, 0);
		}

		int GetHorizontalExtent()
		// Retrieve from the combo box the width, in pixels, by which the list box can 
		// be scrolled horizontally (the scrollable width).
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETHORIZONTALEXTENT, 0, 0);
		}

		DWORD GetItemData(int nIndex)
		// Retrieves the application-supplied value associated with the specified item in the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (DWORD)::SendMessage(m_hWnd, CB_GETITEMDATA, (WPARAM)nIndex, 0);
		}

		int GetItemHeight(int nIndex)
		// Determines the height of list items or the selection field in the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETITEMHEIGHT, (WPARAM)nIndex, 0);
		}

		int GetLBText(int nIndex, LPTSTR lpszText)
		// Retrieves a string from the list of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)lpszText);
		}

		int GetLBTextLen(int nIndex)
		// Retrieves the length, in characters, of a string in the list of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETLBTEXTLEN, (WPARAM)nIndex, 0);
		}

		LCID GetLocale()
		// Retrieves the current locale of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (LCID)::SendMessage(m_hWnd, CB_GETLOCALE, 0, 0);
		}

		int GetTopIndex()
		// Retrieves the zero-based index of the first visible item in the list box portion of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_GETTOPINDEX, 0, 0);
		}

		int InitStorage(int nItems, int nBytes)
		// Allocates memory for storing list box items. Use this before adding a 
		// large number of items to the list box portion of a combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_INITSTORAGE, (WPARAM)nItems, (LPARAM)nBytes);
		}

		int InsertString(int nIndex, LPCTSTR lpszString)
		// Inserts a string into the list box of the combo box. Unlike the AddString, 
		// a list with the CBS_SORT style is not sorted.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_INSERTSTRING, (WPARAM)nIndex, (LPARAM)lpszString);
		}

		void Paste()
		// Copies the current content of the clipboard to the combo box's edit control at the current caret position.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_PASTE, 0, 0);
		}

		void LimitText(int nMaxChars)
		// Limits the length of the text the user may type into the edit control of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, CB_LIMITTEXT, (WPARAM)nMaxChars, 0);
		}

		void ResetContent()
		// Removes all items from the list box and edit control of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0);
		}

		int SelectString(int nStartAfter, LPCTSTR lpszString)
		// Searches the list of a combo box for an item that begins with the characters in a
		// specified string. If a matching item is found, it is selected and copied to the edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_SELECTSTRING, (WPARAM)nStartAfter, (LPARAM)lpszString);
		}

		int SetCurSel(int nIndex)
		// Selects a string in the list of the combo box. If necessary, the list scrolls the string into view.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_SETCURSEL, (WPARAM)nIndex, 0);
		}

		int SetDroppedWidth(int nWidth)
		// Sets the maximum allowable width, in pixels, of the list box of the combo box with
		// the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_SETDROPPEDWIDTH, (WPARAM)nWidth, 0);
		}

		BOOL SetEditSel(int nStartChar, int nEndChar)
		// Selects characters in the edit control of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, CB_SETEDITSEL, 0, (LPARAM)MAKELONG(nStartChar,nEndChar));
		}

		int SetExtendedUI(BOOL bExtended)
		// Selects either the default user interface or the extended user interface for the combo box that 
		// has the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_SETEXTENDEDUI, (WPARAM)bExtended, 0);
		}

		void SetHorizontalExtent(UINT nExtent)
		// Sets the width, in pixels, by which the list box can be scrolled horizontally (the scrollable width).
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, CB_SETHORIZONTALEXTENT, (WPARAM)nExtent, 0);
		}

		int SetItemData(int nIndex, DWORD dwItemData)
		// Sets the value associated with the specified item in the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_SETITEMDATA, (WPARAM)nIndex, (LPARAM)dwItemData);
		}

		int SetItemHeight(int nIndex, UINT cyItemHeight)
		// Sets the height of list items or the selection field in the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_SETITEMHEIGHT, (WPARAM)nIndex, (LPARAM)cyItemHeight);
		}

		LCID SetLocale(LCID NewLocale)
		// Sets the current locale of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (LCID)::SendMessage(m_hWnd, CB_SETLOCALE, (WPARAM)NewLocale, 0);
		}

		int SetTopIndex(int nIndex)
		// Ensure that a particular item is visible in the list box of the combo box.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, CB_SETTOPINDEX, (WPARAM)nIndex, 0);
		}

		void ShowDropDown(BOOL bShow)
		// Shows or hides the list box of the combo box that has the CBS_DROPDOWN or CBS_DROPDOWNLIST style.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, CB_SHOWDROPDOWN, (WPARAM)bShow, 0);
		}

		int GetLBText(int i, String &s)
		{
			int ret = GetLBTextLen(i);
			if (ret != CB_ERR)
			{
				s.resize(ret);
				ret = GetLBText(i, s.pointer());
				s.resize(ret);
			}
			return ret;
		}
	};

	class HComboBox : public ComboBox<HWindow>
	{
	public:
		static HComboBox *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, WC_COMBOBOX, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HComboBox *>(pWnd);
		}
	};

	template<class Super>
	class Edit : public Super
	{
	public:
		BOOL CanUndo()
		// Returns TRUE if the edit control operation can be undone.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0);
		}

		int GetFirstVisibleLine()
		// Returns the zero-based index of the first visible character in a single-line edit control 
		// or the zero-based index of the uppermost visible line in a multiline edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0);
		}

		HLOCAL GetHandle()
		// Returns a handle identifying the buffer containing the multiline edit control's text. 
		// It is not processed by single-line edit controls.
		{
			assert(::IsWindow(m_hWnd));
			return (HLOCAL)::SendMessage(m_hWnd, EM_GETHANDLE, 0, 0);
		}

		UINT GetLimitText()
		// Returns the current text limit, in characters.
		{
			assert(::IsWindow(m_hWnd));
			return (UINT)::SendMessage(m_hWnd, EM_GETLIMITTEXT, 0, 0);
		}

		int GetLine(int nIndex, LPTSTR lpszBuffer)
		// Copies characters to a buffer and returns the number of characters copied.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
		}

		int GetLine(int nIndex, LPTSTR lpszBuffer, int nMaxLength)
		// Copies characters to a buffer and returns the number of characters copied.
		{
			assert(::IsWindow(m_hWnd));
			*(LPWORD)lpszBuffer = (WORD)nMaxLength;
			return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
		}

		int GetLineCount()
		// Returns the number of lines in the edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0);
		}

		DWORD GetMargins()
		// Returns the widths of the left and right margins.
		{
			assert(::IsWindow(m_hWnd));
			return (DWORD)::SendMessage(m_hWnd, EM_GETMARGINS, 0, 0);
		}

		BOOL GetModify()
		// Returns a flag indicating whether the content of an edit control has been modified.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0);
		}

		TCHAR GetPasswordChar()
		// Returns the character that edit controls use in conjunction with the ES_PASSWORD style.
		{
			assert(::IsWindow(m_hWnd));
			return (TCHAR)::SendMessage(m_hWnd, EM_GETPASSWORDCHAR, 0, 0);
		}

		void GetRect(LPRECT lpRect)
		// Returns the coordinates of the formatting rectangle in an edit control.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect);
		}

		void GetSel(int& nStartChar, int& nEndChar)
		// Returns the starting and ending character positions of the current selection in the edit control.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nStartChar,(LPARAM)&nEndChar);
		}

		DWORD GetSel()
		// Returns the starting and ending character positions of the current selection in the edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (DWORD)::SendMessage(m_hWnd, EM_GETSEL, 0, 0);
		}

		void SetHandle(HLOCAL hBuffer)
		// Sets a handle to the memory used as a text buffer, empties the undo buffer, 
		// resets the scroll positions to zero, and redraws the window.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETHANDLE, (WPARAM)hBuffer, 0);
		}

		void SetLimitText(UINT nMax)
		// Sets the maximum number of characters the user may enter in the edit control.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETLIMITTEXT, (WPARAM)nMax, 0);
		}

		void SetMargins(UINT nLeft, UINT nRight)
		// Sets the widths of the left and right margins, and redraws the edit control to reflect the new margins.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELONG(nLeft, nRight));
		}

		void SetModify(BOOL bModified)
		// Sets or clears the modification flag to indicate whether the edit control has been modified.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0);
		}

		void EmptyUndoBuffer()
		// Empties the undo buffer and sets the undo flag retrieved by the EM_CANUNDO message to FALSE.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0);
		}

		BOOL FmtLines(BOOL bAddEOL)
		// Adds or removes soft line-break characters (two carriage returns and a line feed) to the ends of wrapped lines 
		// in a multiline edit control. It is not processed by single-line edit controls.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, EM_FMTLINES, bAddEOL, 0);
		}

		void LimitText(int nChars)
		// Sets the text limit of an edit control. The text limit is the maximum amount of text, in TCHARs, 
		// that the user can type into the edit control.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_LIMITTEXT, nChars, 0);
		}

		int LineFromChar(int nIndex)
		// Returns the zero-based number of the line in a multiline edit control that contains a specified character index.
		// This message is the reverse of the EM_LINEINDEX message.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, EM_LINEFROMCHAR, (WPARAM)nIndex, 0);
		}

		int LineIndex(int nLine)
		// Returns the character of a line in a multiline edit control. 
		// This message is the reverse of the EM_LINEFROMCHAR message
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, EM_LINEINDEX, (WPARAM)nLine, 0);
		}

		int LineLength(int nLine)
		// Returns the length, in characters, of a single-line edit control. In a multiline edit control, 
		//	returns the length, in characters, of a specified line.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, EM_LINELENGTH, (WPARAM)nLine, 0);
		}

		void LineScroll(int nLines, int nChars)
		// Scrolls the text vertically in a single-line edit control or horizontally in a multiline edit control.
		{
			assert(::IsWindow(m_hWnd)); 
			::SendMessage(m_hWnd, EM_LINESCROLL, (WPARAM)nChars, (LPARAM)nLines);
		}

		void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE)
		// Replaces the current selection with the text in an application-supplied buffer, sends the parent window 
		// EN_UPDATE and EN_CHANGE messages, and updates the undo buffer.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText);
		}

		void SetPasswordChar(TCHAR ch)
		// Defines the character that edit controls use in conjunction with the ES_PASSWORD style.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETPASSWORDCHAR, ch, 0);
		}

		BOOL SetReadOnly(BOOL bReadOnly)
		// Sets or removes the read-only style (ES_READONLY) in an edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0);
		}

		void SetRect(LPCRECT lpRect)
		// Sets the formatting rectangle for the multiline edit control and redraws the window.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect);
		}

		void SetRectNP(LPCRECT lpRect)
		// Sets the formatting rectangle for the multiline edit control but does not redraw the window.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETRECTNP, 0, (LPARAM)lpRect);
		}

		void SetSel(DWORD dwSelection)
		// Selects a range of characters in the edit control by setting the starting and ending positions to be selected.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection));
		}

		void SetSel(int nStartChar, int nEndChar)
		// Selects a range of characters in the edit control by setting the starting and ending positions to be selected.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_SETSEL, nStartChar, nEndChar);
		}

		BOOL SetTabStops(int nTabStops, LPINT rgTabStops)
		// Sets tab-stop positions in the multiline edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
		}

		BOOL SetTabStops()
		// Sets tab-stop positions in the multiline edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS, 0, 0);
		}

		BOOL SetTabStops(const int& cxEachStop)
		// Sets tab-stop positions in the multiline edit control.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS, 1, (LPARAM)(LPINT)&cxEachStop);
		}

		void Clear()
		// Clears the current selection, if any, in an edit control.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_CLEAR, 0, 0);
		}

		void Copy()
		// Copies text to the clipboard unless the style is ES_PASSWORD, in which case the message returns zero.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_COPY, 0, 0);
		}

		void Cut()
		// Cuts the selection to the clipboard, or deletes the character to the left of the cursor if there is no selection.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_CUT, 0, 0);
		}

		void Paste()
		// Pastes text from the clipboard into the edit control window at the caret position.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, WM_PASTE, 0, 0);
		}

		void Undo()
		// Removes any text that was just inserted or inserts any deleted characters and sets the selection to the inserted text.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, EM_UNDO, 0, 0);
		}

		// Instructs system edit controls to use AutoComplete to help complete URLs or file system paths.
		HRESULT SHAutoComplete(DWORD dwFlags)
		{
			assert(::IsWindow(m_hWnd));
			return ::SHAutoComplete(m_hWnd, dwFlags);
		}
	};

	class HEdit : public Edit<HWindow>
	{
	public:
		static HEdit *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, WC_EDIT, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HEdit *>(pWnd);
		}
	};

	template<class Super>
	class Static : public Super
	{
	public:
		HBITMAP GetBitmap()
		// Returns the handle to the bitmap for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HBITMAP)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_BITMAP, 0);
		}

		HCURSOR GetCursor()
		// Returns the handle to the icon for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HCURSOR)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_CURSOR, 0);
		}

		HENHMETAFILE GetEnhMetaFile()
		// Returns the handle to the enhanced metafile for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HENHMETAFILE)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_ENHMETAFILE, 0);
		}

		HICON GetIcon()
		// Returns the handle to the icon for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HICON)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_ICON, 0);
		}

		HBITMAP SetBitmap(HBITMAP hBitmap)
		// Sets the handle to the bitmap for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HBITMAP)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		}

		HCURSOR SetCursor(HCURSOR hCursor)
		// Sets the handle to the cursor for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HCURSOR)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor);
		}

		HENHMETAFILE SetEnhMetaFile(HENHMETAFILE hMetaFile)
		// Sets the handle to the enhanced metafile for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HENHMETAFILE)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_ENHMETAFILE, (LPARAM)hMetaFile);
		}

		HICON SetIcon(HICON hIcon)
		// Sets the handle to the icon for the static control
		{
			assert(::IsWindow(m_hWnd));
			return (HICON)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		}
	};

	class HStatic : public Static<HWindow>
	{
	public:
		static HStatic *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, WC_STATIC, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HStatic *>(pWnd);
		}
	};

	template<class Super>
	class Button : public Super
	{
	public:
		HBITMAP GetBitmap()
		// returns the handle to the bitmap associated with the button
		{
			assert(::IsWindow(m_hWnd));
			return (HBITMAP)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_BITMAP, 0);
		}

		UINT GetButtonStyle()
		// returns the style of the button
		{
			assert(::IsWindow(m_hWnd));
			return (UINT)::GetWindowLong(m_hWnd, GWL_STYLE) & 0xff;
		}

		int GetCheck()
		// returns the check state of the button
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, BM_GETCHECK, 0, 0);
		}

		HCURSOR GetCursor()
		// returns the handle to the cursor associated withe the button
		{
			assert(::IsWindow(m_hWnd));
			return (HCURSOR)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_CURSOR, 0L);
		}

		HICON GetIcon()
		// returns the handle to the icon associated withe the button
		{
			assert(::IsWindow(m_hWnd));
			return (HICON)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_ICON, 0);
		}

		UINT GetState()
		// returns the state of the button
		{
			assert(::IsWindow(m_hWnd));
			return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0);
		}

		HBITMAP SetBitmap(HBITMAP hBitmap)
		// sets the bitmap associated with the button
		{
			assert(::IsWindow(m_hWnd));
			return (HBITMAP)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		}

		void SetButtonStyle(DWORD dwStyle, BOOL bRedraw = TRUE)
		// sets the button style
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, BM_SETSTYLE, dwStyle, bRedraw);
		}

		void SetCheck(int nCheckState)
		// sets the button check state
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, BM_SETCHECK, nCheckState, 0);
		}

		HCURSOR SetCursor(HCURSOR hCursor)
		// sets the cursor associated with the button
		{
			assert(::IsWindow(m_hWnd));
			return (HCURSOR)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor);
		}

		HICON SetIcon(HICON hIcon)
		// sets the icon associated with the button
		{
			assert(::IsWindow(m_hWnd));
			return (HICON)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		}

		void SetState(BOOL bHighlight)
		// sets the button state
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0);
		}
	};

	class HButton : public Button<HWindow>
	{
	public:
		static HButton *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id, DWORD xstyle = 0)
		{
			HWindow *pWnd = CreateEx(xstyle, WC_BUTTON, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HButton *>(pWnd);
		}
	};

	template<class Super>
	class HeaderCtrl : public Super
	{
	public:
		int GetItemCount()
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return Header_GetItemCount(m_hWnd);
		}
		BOOL GetItem(int i, HD_ITEM *phdi)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return Header_GetItem(m_hWnd, i, phdi);
		}
		BOOL SetItem(int i, const HD_ITEM *phdi)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return Header_SetItem(m_hWnd, i, phdi);
		}
	};

	class HHeaderCtrl : public HeaderCtrl<HWindow>
	{
	public:
		static HHeaderCtrl *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, WC_HEADER, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HHeaderCtrl *>(pWnd);
		}
	};

	template<class Super>
	class ListView : public Super
	{
	public:
		DWORD ApproximateViewRect(int iWidth, int iHeight, int iCount = -1)
		// Calculates the approximate width and height required to display a given number of items.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_ApproximateViewRect(m_hWnd, iWidth, iHeight, iCount);
		}

		COLORREF GetBkColor()
		// Retrieves the background color of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetBkColor(m_hWnd);
		}

		BOOL GetBkImage(LVBKIMAGE *lvbkImage)
		// Retrieves the background image in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetBkImage(m_hWnd, lvbkImage);
		}

		UINT GetCallbackMask()
		// Retrieves the callback mask for a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetCallbackMask(m_hWnd);
		}

		BOOL GetCheck(UINT nItem)
		// Determines if an item in a list-view control is selected.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetCheckState(m_hWnd, nItem);
		}

		BOOL GetColumn(int iCol, LVCOLUMN *Column)
		// Retrieves the attributes of a list-view control's column.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetColumn(m_hWnd, iCol, Column);
		}

		BOOL GetColumnOrderArray(LPINT piArray, int iCount = -1)
		// Retrieves the current left-to-right order of columns in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetColumnOrderArray(m_hWnd, iCount, piArray);
		}

		int GetColumnWidth(int iCol)
		// Retrieves the width of a column in report or list view.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetColumnWidth(m_hWnd, iCol);
		}

		int GetCountPerPage()
		// Calculates the number of items that can fit vertically in the visible area of a
		// list-view control when in list or report view. Only fully visible items are counted.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetCountPerPage(m_hWnd);
		}

		HHeaderCtrl *GetHeaderCtrl()
		// Retrieves the handle to the header control used by a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HHeaderCtrl *>(ListView_GetHeader(m_hWnd));
		}

		HEdit *GetEditControl()
		// Retrieves the handle to the edit control being used to edit a list-view item's text.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return reinterpret_cast<HEdit *>(ListView_GetEditControl(m_hWnd));
		}

		DWORD GetExtendedStyle()
		// Retrieves the extended styles that are currently in use for a given list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetExtendedListViewStyle(m_hWnd);
		}

		HCURSOR GetHotCursor()
		// Retrieves the HCURSOR used when the pointer is over an item while hot tracking is enabled.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetHotCursor(m_hWnd);
		}

		int GetHotItem()
		// Retrieves the index of the hot item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetHotItem(m_hWnd);
		}

		DWORD GetHoverTime()
		// Retrieves the amount of time that the mouse cursor must hover over an item before it is selected.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetHoverTime(m_hWnd);
		}

		HIMAGELIST GetImageList(int nImageType)
		// Retrieves the handle to an image list used for drawing list-view items.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetImageList(m_hWnd, nImageType);
		}

		BOOL GetItem(LVITEM *Item)
		// Retrieves some or all of a list-view item's attributes.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetItem(m_hWnd, Item);
		}

		int GetItemCount()
		// Retrieves the number of items in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetItemCount(m_hWnd);
		}

		DWORD_PTR GetItemData(int iItem)
		// Retrieves the value(lParam) specific to the item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));

			LVITEM lvi;
			lvi.iItem = iItem;
			lvi.iSubItem = 0;
			lvi.mask = LVIF_PARAM;
			ListView_GetItem(m_hWnd, &lvi);
			return lvi.lParam;
		}

		BOOL GetItemPosition(int iItem, LPPOINT ppt)
		// Retrieves the position of a list-view item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetItemPosition(m_hWnd, iItem, ppt);
		}

		BOOL GetItemRect(int iItem, LPRECT prc, UINT nCode)
		// Retrieves the bounding rectangle for all or part of an item in the current view.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetItemRect(m_hWnd, iItem, prc, nCode);
		}

		UINT GetItemState(int iItem, UINT nMask)
		// Retrieves the state of a list-view item.

		// Possible values of nMask:
		// LVIS_CUT				The item is marked for a cut-and-paste operation.
		// LVIS_DROPHILITED		The item is highlighted as a drag-and-drop target.
		// LVIS_FOCUSED			The item has the focus, so it is surrounded by a standard focus rectangle.
		// LVIS_SELECTED		The item is selected.
		// LVIS_OVERLAYMASK		Use this mask to retrieve the item's overlay image index.
		// LVIS_STATEIMAGEMASK	Use this mask to retrieve the item's state image index.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetItemState(m_hWnd, iItem, nMask);
		}

		void GetItemText(int iItem, int iSubItem, LPTSTR pszText, int cchTextMax)
		// Retrieves the text of a list-view item.
		// Note: Although the list-view control allows any length string to be stored
		//       as item text, only the first 260 characters are displayed.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_GetItemText(m_hWnd, iItem, iSubItem, pszText, cchTextMax);
		}

		String GetItemText(int iItem, int iSubItem, UINT cchTextMax = 260)
		// Retrieves the text of a list-view item.
		// Note: Although the list-view control allows any length string to be stored
		//       as item text, only the first 260 characters are displayed.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			LVITEM lvi;
			lvi.iItem = iItem;
			lvi.iSubItem = iSubItem;
			lvi.mask = LVIF_TEXT;
			lvi.cchTextMax = cchTextMax;
			lvi.pszText = (LPTSTR)_alloca(cchTextMax * sizeof(TCHAR));
			if (!ListView_GetItem(m_hWnd, &lvi))
				lvi.pszText[0] = _T('\0');
			return lvi.pszText;
		}

		int GetNextItem(int iItem, int iFlags)
		// Searches for a list-view item that has the specified properties and
		// bears the specified relationship to a specified item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetNextItem(m_hWnd, iItem, iFlags);
		}

		UINT GetNumberOfWorkAreas()
		// Retrieves the working areas from a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			UINT nWorkAreas = 0;
			ListView_GetWorkAreas(m_hWnd, nWorkAreas, NULL);
			return nWorkAreas;
		}

		BOOL GetOrigin(LPPOINT ppt)
		// Retrieves the current view origin for a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetOrigin(m_hWnd, ppt);
		}

		UINT GetSelectedCount()
		// Determines the number of selected items in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetSelectedCount(m_hWnd);
		}

		int GetSelectionMark()
		// Retrieves the selection mark from a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetSelectionMark(m_hWnd);
		}

		int GetStringWidth(LPCTSTR pszString)
		// Determines the width of a specified string using the specified list-view control's current font.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetStringWidth(m_hWnd, pszString);
		}

		BOOL GetSubItemRect(int iItem, int iSubItem, int iCode, LPRECT prc)
		// Retrieves information about the rectangle that surrounds a subitem in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetSubItemRect(m_hWnd, iItem, iSubItem, iCode, prc);
		}

		COLORREF GetTextBkColor()
		// Retrieves the text background color of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetTextBkColor(m_hWnd);
		}

		COLORREF GetTextColor()
		// Retrieves the text color of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetTextColor(m_hWnd);
		}

		HWND GetToolTips()
		// Retrieves the ToolTip control that the list-view control uses to display ToolTips.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetToolTips(m_hWnd);
		}

		int GetTopIndex()
		// Retrieves the index of the topmost visible item when in list or report view.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetTopIndex(m_hWnd);
		}

		BOOL GetViewRect(LPRECT prc)
		// Retrieves the bounding rectangle of all items in the list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_GetViewRect(m_hWnd, prc);
		}

		void GetWorkAreas(int iWorkAreas, LPRECT pRectArray)
		// Retrieves the working areas from a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_GetWorkAreas(m_hWnd, iWorkAreas, pRectArray);
		}

		BOOL SetBkColor(COLORREF clrBk)
		// Sets the background color of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetBkColor(m_hWnd, clrBk);
		}

		BOOL SetBkImage(LVBKIMAGE *lvbkImage)
		// Sets the background image in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetBkImage(m_hWnd, lvbkImage);
		}

		BOOL SetCallbackMask(UINT nMask)
		// Changes the callback mask for a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetCallbackMask(m_hWnd, nMask);
		}

		void SetCheck(int iItem, BOOL fCheck = TRUE)
		// Used to select or deselect an item in a list-view control.
		// This macro should only be used for list-view controls with the LVS_EX_CHECKBOXES style.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_SetCheckState(m_hWnd, iItem, fCheck);
		}

		BOOL SetColumn(int iCol, const LVCOLUMN *Column)
		// Sets the attributes of a list-view column.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetColumn(m_hWnd, iCol, Column);
		}

		BOOL SetColumnOrderArray(int iCount, LPINT piArray)
		// Sets the left-to-right order of columns in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetColumnOrderArray(m_hWnd, iCount, piArray);
		}

		BOOL SetColumnWidth(int iCol, int cx)
		// Used to change the width of a column in report view or the width of all columns in list-view mode.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetColumnWidth(m_hWnd, iCol, cx);
		}

		DWORD SetExtendedStyle(DWORD dwNewStyle)
		// Sets extended styles for list-view controls.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetExtendedListViewStyle(m_hWnd, dwNewStyle);
		}

		HCURSOR SetHotCursor(HCURSOR hCursor)
		// Sets the HCURSOR that the list-view control uses when the pointer is
		// over an item while hot tracking is enabled.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetHotCursor(m_hWnd, hCursor);
		}

		int SetHotItem(int nIndex)
		// Sets the hot item in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetHotItem(m_hWnd, nIndex);
		}

		DWORD SetHoverTime(DWORD dwHoverTime = (DWORD)-1)
		// Sets the amount of time that the mouse cursor must hover over an item before it is selected.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetHoverTime(m_hWnd, dwHoverTime);
		}

		DWORD SetIconSpacing(int cx, int cy)
		// Sets the spacing between icons in list-view controls set to the LVS_ICON style.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetIconSpacing(m_hWnd, cx, cy);
		}

		HIMAGELIST SetImageList(HIMAGELIST himl, int iImageListType)
		// Assigns an image list to a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetImageList(m_hWnd, himl, iImageListType);
		}

		BOOL SetItem(LVITEM *Item)
		// Sets some or all of a list-view item's attributes.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetItem(m_hWnd, Item);
		}

		BOOL SetItem(int iItem, int iSubItem, UINT nMask, LPCTSTR pszText, int iImage,
						UINT nState, UINT nStateMask, LPARAM lParam, int iIndent)
		// Sets some or all of a list-view item's attributes.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));

			LVITEM lvi;
			lvi.iItem = iItem;
			lvi.iSubItem = iSubItem;
			lvi.mask = nMask;
			lvi.pszText = const_cast<LPTSTR>(pszText);
			lvi.iImage = iImage;
			lvi.state = nState;
			lvi.stateMask = nStateMask;
			lvi.lParam = lParam;
			lvi.iIndent = iIndent;

			return ListView_SetItem(m_hWnd, &lvi);
		}

		void SetItemCount(int iCount)
		// Causes the list-view control to allocate memory for the specified number of items.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_SetItemCount(m_hWnd, iCount);
		}

		void SetItemCountEx(int iCount, DWORD dwFlags = LVSICF_NOINVALIDATEALL)
		// Sets the virtual number of items in a virtual list view.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_SetItemCountEx(m_hWnd, iCount, dwFlags);
		}

		BOOL SetItemData(int iItem, DWORD_PTR dwData)
		// Sets the value(lParam) specific to the item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			LVITEM lvi;
			lvi.iItem = iItem;
			lvi.iSubItem = 0;
			lvi.lParam = dwData;
			lvi.mask = LVIF_PARAM;
			return ListView_SetItem(m_hWnd, &lvi);
		}

		BOOL SetItemPosition(int iItem, int x, int y)
		// Moves an item to a specified position in a list-view control (in icon or small icon view).
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetItemPosition(m_hWnd, iItem, x, y);
		}

		BOOL SetItemState(int iItem, LVITEM *Item)
		// Changes the state of an item in a list-view control.

		// Possible values of nMask:
		// LVIS_CUT				The item is marked for a cut-and-paste operation.
		// LVIS_DROPHILITED		The item is highlighted as a drag-and-drop target.
		// LVIS_FOCUSED			The item has the focus, so it is surrounded by a standard focus rectangle.
		// LVIS_SELECTED		The item is selected.
		// LVIS_OVERLAYMASK		Use this mask to retrieve the item's overlay image index.
		// LVIS_STATEIMAGEMASK	Use this mask to retrieve the item's state image index.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMSTATE, (WPARAM)iItem, (LPARAM)Item);
		}

		void SetItemState(int iItem, UINT nState, UINT nMask)
		// Changes the state of an item in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_SetItemState(m_hWnd, iItem, nState, nMask);
		}

		void SetItemText(int iItem, int iSubItem, LPCTSTR pszText)
		// Sets the text color of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_SetItemText(m_hWnd, iItem, iSubItem, (LPTSTR)pszText);
		}

		int SetSelectionMark(int iIndex)
		// Sets the selection mark in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetSelectionMark(m_hWnd, iIndex);
		}

		BOOL SetTextBkColor(COLORREF clrBkText)
		// Sets the background color of text in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetTextBkColor(m_hWnd, clrBkText);
		}

		BOOL SetTextColor(COLORREF clrText)
		// Sets the text color of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SetTextColor(m_hWnd, clrText);
		}

		HWND SetToolTips(HWND hWndToolTip)
		// Sets the ToolTip control that the list-view control will use to display ToolTips.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return (HWND)::SendMessage(m_hWnd, LVM_SETTOOLTIPS, (WPARAM)hWndToolTip, 0L);
		}

		void SetWorkAreas(int nWorkAreas, LPRECT pRectArray)
		// Sets the working area within a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_SetWorkAreas(m_hWnd, nWorkAreas, pRectArray);
		}

		int SubItemHitTest(LVHITTESTINFO *htInfo)
		// Determines which list-view item or subitem is located at a given position.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SubItemHitTest(m_hWnd, htInfo);
		}

		// Operations

		BOOL Arrange(UINT nCode)
		// Arranges items in icon view.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_Arrange(m_hWnd, nCode);
		}

		HIMAGELIST CreateDragImage(int iItem, LPPOINT ppt)
		// Creates a drag image list for the specified item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_CreateDragImage(m_hWnd, iItem, ppt);
		}

		BOOL DeleteAllItems()
		// ListView_DeleteAllItems
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_DeleteAllItems(m_hWnd);
		}

		BOOL DeleteColumn(int iCol)
		// Removes a column from a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_DeleteColumn(m_hWnd, iCol);
		}

		BOOL DeleteItem(int iItem)
		// Removes an item from a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_DeleteItem(m_hWnd, iItem);
		}

		HWND EditLabel(int iItem)
		// Begins in-place editing of the specified list-view item's text.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_EditLabel(m_hWnd, iItem);
		}

		void CancelEditLabel()
		// Cancels the editing of a list-view item's label.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			ListView_CancelEditLabel(m_hWnd);
		}

		BOOL EnsureVisible(int iItem, BOOL fPartialOK)
		// Ensures that a list-view item is either entirely or partially visible,
		// scrolling the list-view control if necessary.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_EnsureVisible(m_hWnd, iItem, fPartialOK);
		}

		int FindItem(LVFINDINFO *FindInfo, int iStart = -1)
		// Searches for a list-view item with the specified characteristics.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_FindItem(m_hWnd, iStart, FindInfo);
		}

		int HitTest(LVHITTESTINFO *HitTestInfo)
		// Determines which list-view item, if any, is at a specified position.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_HitTest(m_hWnd, HitTestInfo);
		}

		int InsertColumn(int iCol, const LVCOLUMN *Column)
		// Inserts a new column in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_InsertColumn(m_hWnd, iCol, Column);
		}

		int InsertColumn(int iCol, LPCTSTR pszColumnHeading, int iFormat = LVCFMT_LEFT,
							int iWidth = -1, int iSubItem = -1)
		// Inserts a new column in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			LVCOLUMN lvc;
			lvc.mask = LVCF_TEXT | LVCF_ORDER | LVCF_FMT;
			if (-1 != iWidth)
			{
				lvc.mask |= LVCF_WIDTH; 
				lvc.cx = iWidth; 
			}
			if (-1 != iSubItem)
			{
				lvc.mask |= LVCF_SUBITEM; 
				lvc.iSubItem = iSubItem;
			}
			lvc.iOrder = iCol;
			lvc.pszText = const_cast<LPTSTR>(pszColumnHeading);
			lvc.fmt = iFormat;
			lvc.iSubItem = iSubItem;
			return ListView_InsertColumn(m_hWnd, iCol, &lvc);
		}

		int InsertItem(const LVITEM *Item)
		// Inserts a new item in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_InsertItem(m_hWnd, Item);
		}

		int InsertItem(int iItem, LPCTSTR pszText)
		// Inserts a new item in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			LVITEM lvi;
			lvi.iItem = iItem;
			lvi.iSubItem = 0;
			lvi.pszText = const_cast<LPTSTR>(pszText);
			lvi.mask = LVIF_TEXT;
			return ListView_InsertItem(m_hWnd, &lvi);
		}

		int InsertItem(int iItem, LPCTSTR pszText, int iImage)
		// Inserts a new item in a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			LVITEM lvi;
			lvi.iItem = iItem;
			lvi.iSubItem = 0;
			lvi.pszText = const_cast<LPTSTR>(pszText);
			lvi.iImage = iImage;
			lvi.mask = LVIF_TEXT | LVIF_IMAGE;
			return ListView_InsertItem(m_hWnd, &lvi);
		}

		BOOL RedrawItems(int iFirst, int iLast)
		// Forces a list-view control to redraw a range of items.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_RedrawItems(m_hWnd, iFirst, iLast);
		}

		BOOL Scroll(int dx, int dy)
		// Scrolls the content of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_Scroll(m_hWnd, dx, dy);
		}

		BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData)
		// Uses an application-defined comparison function to sort the items of a list-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_SortItems(m_hWnd, pfnCompare, dwData);
		}

		BOOL Update(int iItem)
		// Updates a list-view item. If the list-view control has the LVS_AUTOARRANGE style,
		// the list-view control is rearranged.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return ListView_Update(m_hWnd, iItem);
		}
	};

	class HListView : public ListView<HWindow>
	{
	public:
		static HListView *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id, DWORD xstyle = 0)
		{
			HWindow *pWnd = CreateEx(xstyle, WC_LISTVIEW, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HListView *>(pWnd);
		}
	};

	template<class Super>
	class TreeView : public Super
	{
	public:
		COLORREF GetBkColor()
		// Retrieves the current background color of the control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetBkColor(m_hWnd);
		}

		HTREEITEM GetChildItem(HTREEITEM hItem)
		// Retrieves the first child item of the specified tree-view item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetChild(m_hWnd, hItem);
		}

		UINT GetCount()
		// Retrieves a count of the items in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetCount(m_hWnd);
		}

		HTREEITEM GetDropHiLightItem()
		// Retrieves the tree-view item that is the target of a drag-and-drop operation.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetDropHilight(m_hWnd);
		}

		HWND GetEditControl()
		// Retrieves the handle to the edit control being used to edit a tree-view item's text.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetEditControl(m_hWnd);
		}

		HTREEITEM GetFirstVisible()
		// Retrieves the first visible item in a tree-view control window.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetFirstVisible(m_hWnd);
		}

		HIMAGELIST GetImageList(int iImageType)
		// Retrieves the handle to the normal or state image list associated with a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetImageList(m_hWnd, iImageType);
		}

		UINT GetIndent()
		// Retrieves the amount, in pixels, that child items are indented relative to their parent items.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetIndent(m_hWnd);
		}

		COLORREF GetInsertMarkColor()
		// Retrieves the color used to draw the insertion mark for the tree view.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetInsertMarkColor(m_hWnd);
		}

		BOOL GetItem(TVITEM *Item)
		// Retrieves some or all of a tree-view item's attributes.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetItem(m_hWnd, Item);
		}

		DWORD_PTR GetItemData(HTREEITEM hItem)
		// Retrieves a tree-view item's application data.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TVITEM tvi;
			tvi.mask = TVIF_PARAM;
			tvi.hItem = hItem;
			if (!TreeView_GetItem(m_hWnd, &tvi))
				tvi.lParam = 0;
			return tvi.lParam;
		}

		UINT GetItemState(HTREEITEM hItem, UINT nMask)
		// Retrieves the state of a tree-view item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetItemState(m_hWnd, hItem, nMask);
		}

		int GetItemHeight()
		// Retrieves the current height of the tree-view item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetItemHeight(m_hWnd);
		}

		BOOL GetItemRect(HTREEITEM hItem, LPRECT prc, BOOL bTextOnly)
		// Retrieves the bounding rectangle for a tree-view item and indicates whether the item is visible.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetItemRect(m_hWnd, hItem, prc, bTextOnly);
		}

		String GetItemText(HTREEITEM hItem, UINT cchTextMax = 260)
		// Retrieves the text for a tree-view item.
		// Note: Although the tree-view control allows any length string to be stored 
		//       as item text, only the first 260 characters are displayed.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TVITEM tvi;
			tvi.hItem = hItem;
			tvi.mask = TVIF_TEXT;
			tvi.cchTextMax = cchTextMax;
			tvi.pszText = (LPTSTR)_alloca(cchTextMax * sizeof(TCHAR));
			if (!TreeView_GetItem(m_hWnd, &tvi))
				tvi.pszText[0] = _T('\0');
			return tvi.pszText;
		}

		HTREEITEM GetLastVisibleItem()
		// Retrieves the last expanded item in a tree-view control.
		// This does not retrieve the last item visible in the tree-view window.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetLastVisible(m_hWnd);
		}

		HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode)
		// Retrieves the tree-view item that bears the specified relationship to a specified item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetNextItem(m_hWnd, hItem, nCode);
		}

		HTREEITEM GetNextSiblingItem(HTREEITEM hItem)
		// Retrieves the next sibling item of a specified item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetNextSibling(m_hWnd, hItem);
		}

		HTREEITEM GetNextVisibleItem(HTREEITEM hItem)
		// Retrieves the next visible item that follows a specified item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetNextVisible(m_hWnd, hItem);
		}

		HTREEITEM GetParentItem(HTREEITEM hItem)
		// Retrieves the parent item of the specified tree-view item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetParent(m_hWnd, hItem);
		}

		HTREEITEM GetPrevSiblingItem(HTREEITEM hItem)
		// Retrieves the previous sibling item of a specified item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetPrevSibling(m_hWnd, hItem);
		}

		HTREEITEM GetPrevVisibleItem(HTREEITEM hItem)
		// Retrieves the first visible item that precedes a specified item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetPrevSibling(m_hWnd, hItem);
		}

		HTREEITEM GetRootItem()
		// Retrieves the topmost or very first item of the tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetRoot(m_hWnd);
		}

		int GetScrollTime()
		// Retrieves the maximum scroll time for the tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetScrollTime(m_hWnd);
		}

		HTREEITEM GetSelection()
		// Retrieves the currently selected item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetSelection(m_hWnd);
		}

		COLORREF GetTextColor()
		// Retrieves the current text color of the control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetTextColor(m_hWnd);
		}

		HWND GetToolTips()
		// Retrieves the handle to the child ToolTip control used by a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetToolTips(m_hWnd);
		}

		UINT GetVisibleCount()
		// Obtains the number of items that can be fully visible in the client window of a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetVisibleCount(m_hWnd);
		}

		BOOL ItemHasChildren(HTREEITEM hItem)
		// Returns true of the tree-view item has one or more children
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_GetChild(m_hWnd, hItem) != NULL;
		}

		COLORREF SetBkColor(COLORREF clrBk)
		// Sets the background color of the control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetBkColor(m_hWnd, clrBk);
		}

		HIMAGELIST SetImageList(HIMAGELIST himl, int nType)
		// Sets the normal or state image list for a tree-view control
		// and redraws the control using the new images.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetImageList(m_hWnd, himl, nType);
		}

		void SetIndent(int indent)
		// Sets the width of indentation for a tree-view control
		// and redraws the control to reflect the new width.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TreeView_SetIndent(m_hWnd, indent);
		}

		BOOL SetInsertMark(HTREEITEM hItem, BOOL fAfter = TRUE)
		// Sets the insertion mark in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetInsertMark(m_hWnd, hItem, fAfter);
		}

		COLORREF SetInsertMarkColor(COLORREF clrInsertMark)
		// Sets the color used to draw the insertion mark for the tree view.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetInsertMarkColor(m_hWnd, clrInsertMark);
		}

		BOOL SetItem(TVITEM *Item)
		// Sets some or all of a tree-view item's attributes.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetItem(m_hWnd, Item);
		}

		BOOL SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR szText, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam)
		// Sets some or all of a tree-view item's attributes.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TVITEM tvi;
			tvi.hItem = hItem;
			tvi.mask = nMask;
			tvi.pszText = const_cast<LPTSTR>(szText);
			tvi.iImage = nImage;
			tvi.iSelectedImage = nSelectedImage;
			tvi.state = nState;
			tvi.stateMask = nStateMask;
			tvi.lParam = lParam;
			return TreeView_SetItem(m_hWnd, &tvi);
		}

		BOOL SetItemData(HTREEITEM hItem, DWORD_PTR dwData)
		// Sets the tree-view item's application data.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TVITEM tvi;
			tvi.hItem = hItem;
			tvi.mask = TVIF_PARAM;
			tvi.lParam = dwData;
			return TreeView_SetItem(m_hWnd, &tvi);
		}

		BOOL SetItemState(HTREEITEM hItem, UINT nData, UINT nMask)
		// Changes the state of an item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TVITEM tvi;
			tvi.hItem = hItem;
			tvi.mask = TVIF_STATE;
			tvi.state = nData;
			tvi.stateMask = nMask;
			return TreeView_SetItem(m_hWnd, &tvi);
		}

		int SetItemHeight(SHORT cyItem)
		// Sets the height of the tree-view items.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetItemHeight(m_hWnd, cyItem);
		}

		BOOL SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage)
		// Sets the tree-view item's application image.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TVITEM tvi;
			tvi.hItem = hItem;
			tvi.iImage = nImage;
			tvi.iSelectedImage = nSelectedImage;
			tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			return TreeView_SetItem(m_hWnd, &tvi);
		}

		BOOL SetItemText(HTREEITEM hItem, LPCTSTR szText)
		// Sets the tree-view item's application text.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TVITEM tvi;
			tvi.hItem = hItem;
			tvi.pszText = const_cast<LPTSTR>(szText);
			tvi.mask = TVIF_TEXT;
			return TreeView_SetItem(m_hWnd, &tvi);
		}

		UINT SetScrollTime(UINT uScrollTime)
		// Sets the maximum scroll time for the tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetScrollTime(m_hWnd, uScrollTime);
		}

		COLORREF SetTextColor(COLORREF clrText)
		// Sets the text color of the control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetTextColor(m_hWnd, clrText);
		}

		HWND SetToolTips(HWND hwndTooltip)
		// Sets a tree-view control's child ToolTip control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SetToolTips(m_hWnd, hwndTooltip);
		}

		// Operations

		HIMAGELIST CreateDragImage(HTREEITEM hItem)
		// Creates a dragging bitmap for the specified item in a tree-view control.
		// It also creates an image list for the bitmap and adds the bitmap to the image list.
		// An application can display the image when dragging the item by using the image list functions.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_CreateDragImage(m_hWnd, hItem);
		}

		BOOL DeleteAllItems()
		// Deletes all items from a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_DeleteAllItems(m_hWnd);
		}

		BOOL DeleteItem(HTREEITEM hItem)
		// Removes an item and all its children from a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_DeleteItem(m_hWnd, hItem);
		}

		HWND EditLabel(HTREEITEM hItem)
		// Begins in-place editing of the specified item's text, replacing the text of the item
		// with a single-line edit control containing the text.
		// The specified item is implicitly selected and focused.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_EditLabel(m_hWnd, hItem);
		}

		BOOL EndEditLabelNow(BOOL fCancel)
		// Ends the editing of a tree-view item's label.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_EndEditLabelNow(m_hWnd, fCancel);
		}

		BOOL EnsureVisible(HTREEITEM hItem)
		// Ensures that a tree-view item is visible, expanding the parent item or
		// scrolling the tree-view control, if necessary.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_EnsureVisible(m_hWnd, hItem);
		}

		BOOL Expand(HTREEITEM hItem, UINT nCode)
		// The TreeView_Expand macro expands or collapses the list of child items associated
		// with the specified parent item, if any.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_Expand(m_hWnd, hItem, nCode);
		}

		HTREEITEM HitTest(TVHITTESTINFO *ht)
		// Determines the location of the specified point relative to the client area of a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_HitTest(m_hWnd, ht);
		}

		HTREEITEM InsertItem(TVINSERTSTRUCT *tvis)
		// Inserts a new item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_InsertItem(m_hWnd, tvis);
		}

		BOOL Select(HTREEITEM hitem, UINT flag)
		// Selects the specified tree-view item, scrolls the item into view, or redraws
		// the item in the style used to indicate the target of a drag-and-drop operation.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_Select(m_hWnd, hitem, flag);
		}

		BOOL SelectDropTarget(HTREEITEM hItem)
		// Redraws a specified tree-view control item in the style used to indicate the
		// target of a drag-and-drop operation.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SelectDropTarget(m_hWnd, hItem);
		}

		BOOL SelectItem(HTREEITEM hItem)
		// Selects the specified tree-view item.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SelectItem(m_hWnd, hItem);
		}

		BOOL SelectSetFirstVisible(HTREEITEM hItem)
		// Scrolls the tree-view control vertically to ensure that the specified item is visible.
		// If possible, the specified item becomes the first visible item at the top of the control's window.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SelectSetFirstVisible(m_hWnd, hItem);
		}

		BOOL SortChildren(HTREEITEM hItem, BOOL fRecurse)
		// Sorts the child items of the specified parent item in a tree-view control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SortChildren(m_hWnd, hItem, fRecurse);
		}

		BOOL SortChildrenCB(TVSORTCB *sort, BOOL fRecurse)
		// Sorts tree-view items using an application-defined callback function that compares the items.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TreeView_SortChildrenCB(m_hWnd, sort, fRecurse);
		}
	};

	class HTreeView : public TreeView<HWindow>
	{
	public:
		static HTreeView *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, WC_TREEVIEW, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HTreeView *>(pWnd);
		}
	};

	template<class Super>
	class ToolTips : public Super
	{
	public:
		BOOL AddTool(TOOLINFO *pti)
		{
			assert(::IsWindow(m_hWnd));
			assert(pti->cbSize == TTTOOLINFO_V1_SIZE || pti->cbSize == sizeof(*pti));
			return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(pti));
		}
		void Activate(BOOL bActive = TRUE)
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, TTM_ACTIVATE, bActive, 0);
		}
		int GetMaxTipWidth()
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, TTM_GETMAXTIPWIDTH, 0, 0);
		}
		int SetMaxTipWidth(int width)
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, width);
		}
	};

	class HToolTips : public ToolTips<HWindow>
	{
	public:
		static HToolTips *Create(DWORD style, HWindow *parent = NULL)
		{
			HWindow *pWnd = CreateEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, 0);
			return static_cast<HToolTips *>(pWnd);
		}
	};

	template<class Super>
	class ToolBar : public Super
	{
	public:
		void ButtonStructSize(UINT cbStruct = sizeof(TBBUTTON))
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, cbStruct, 0);
		}

		BOOL SetBitmapSize(int cx, int cy)
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, TB_SETBITMAPSIZE, 0, MAKELPARAM(cx, cy));
		}

		BOOL SetButtonSize(int cx, int cy)
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(cx, cy));
		}

		BOOL AddButtons(UINT uNumButtons, TBBUTTON *lpButtons)
		// Adds one or more buttons to a toolbar.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, TB_ADDBUTTONS, uNumButtons, (LPARAM)lpButtons);
		}

		int AddString(HINSTANCE hinst, UINT id)
		// Adds a new string, passed as a resource ID, to the toolbar's internal list of strings.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, TB_ADDSTRING, (WPARAM)hinst, (LPARAM)id);
		}

		int AddStrings(LPCTSTR lpszStrings)
		// Adds a new string or strings to the list of strings available for a toolbar control.
		// Strings in the buffer must be separated by a null character. You must ensure that the last string has two null terminators.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, TB_ADDSTRING, 0, (LPARAM)lpszStrings);
		}

		void Autosize()
		// Causes a toolbar to be resized.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0);
		}

		void CheckButton(int idButton, BOOL fCheck)
		// Checks or unchecks a given button in a toolbar.
		// When a button is checked, it is displayed in the pressed state.
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, TB_CHECKBUTTON, (WPARAM)idButton, fCheck);
		}

		int CommandToIndex(int idButton)
		// Retrieves the zero-based index for the button associated with the specified command identifier
		{
			assert(::IsWindow(m_hWnd));
			// returns -1 on fail
			return (int)::SendMessage(m_hWnd, TB_COMMANDTOINDEX, (WPARAM)idButton, 0);
		}

		BOOL DeleteButton(int iButton)
		// Deletes a button from the toolbar.
		// iButton is the Zero-based index of the button to delete.
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
		}

		BOOL EnableButton(int idButton, BOOL fEnable = TRUE)
		// Enables the specified button in a toolbar
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, TB_ENABLEBUTTON, (WPARAM)idButton, fEnable);
		}

		BOOL GetButton(int iButton, LPTBBUTTON lpButton)
		// Recieves the TBBUTTON structure information from the specified button
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, TB_GETBUTTON, (WPARAM)iButton, (LPARAM)lpButton);
		}

		int GetButtonCount()
		// Retrieves a count of the buttons currently in the toolbar
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0);
		}

		DWORD GetButtonSize()
		// Retrieves the current width and height of toolbar buttons, in pixels.
		// Returns a DWORD value that contains the width and height values in the low word and high word, respectively.
		{
			assert(::IsWindow(m_hWnd));
			return (DWORD)::SendMessage(m_hWnd, TB_GETBUTTONSIZE, 0, 0);
		}

		UINT GetButtonState(int idButton)
		// Get the state of an individual button
		//	TBSTATE_CHECKED		The button has the TBSTYLE_CHECK style and is being clicked.
		//	TBSTATE_ELLIPSES	The button's text is cut off and an ellipsis is displayed.
		//	TBSTATE_ENABLED		The button accepts user input. A button that doesn't have this state is grayed.
		//	TBSTATE_HIDDEN		The button is not visible and cannot receive user input.
		//	TBSTATE_INDETERMINATE	The button is grayed.
		//	TBSTATE_MARKED		The button is marked. The interpretation of a marked item is dependent upon the application.
		//	TBSTATE_PRESSED		The button is being clicked.
		//	TBSTATE_WRAP		The button is followed by a line break.
		{
			assert(::IsWindow(m_hWnd));
			return (UINT)::SendMessage(m_hWnd, TB_GETSTATE, (WPARAM)idButton, 0);
		}

		BOOL SetButtonState(int idButton, UINT uState)
		// Set the state of an individual button
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, TB_SETSTATE, (WPARAM)idButton, MAKELPARAM(uState, 0));
		}

		HIMAGELIST SetImageList(HIMAGELIST himlNew)
		// Sets the image list that the toolbar will use to display buttons that are in their default state.
		{
			assert(::IsWindow(m_hWnd));
			return (HIMAGELIST)::SendMessage(m_hWnd, TB_SETIMAGELIST, 0L, (LPARAM)himlNew);
		}

		HIMAGELIST SetDisabledImageList(HIMAGELIST himlNew)
		// Sets the image list that the toolbar control will use to display disabled buttons.
		{
			assert(::IsWindow(m_hWnd));
			return (HIMAGELIST)::SendMessage(m_hWnd, TB_SETDISABLEDIMAGELIST, 0L, (LPARAM)himlNew);
		}

		int GetButtonInfo(UINT id, TBBUTTONINFO *pButtonInfo)
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, TB_GETBUTTONINFO, id, (LPARAM)pButtonInfo);
		}

		BOOL SetButtonInfo(UINT id, TBBUTTONINFO *pButtonInfo)
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, TB_SETBUTTONINFO, id, (LPARAM)pButtonInfo);
		}

	};

	class HToolBar : public ToolBar<HWindow>
	{
	public:
		static HToolBar *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, TOOLBARCLASSNAME, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HToolBar *>(pWnd);
		}
	};

	template<class Super>
	class StatusBar : public Super
	{
	public:
		BOOL SetParts(int iParts, const int *iPaneWidths)
		// Sets the number of parts in a status window and the coordinate of the right edge of each part. 
		// If an element of iPaneWidths is -1, the right edge of the corresponding part extends
		// to the border of the window
		{
			assert(::IsWindow(m_hWnd));
			assert(iParts <= 256);	
			return (BOOL)::SendMessage(m_hWnd, SB_SETPARTS, iParts, (LPARAM)iPaneWidths);		
		}

		int GetParts()
		{
			assert(::IsWindow(m_hWnd));
			return (int)::SendMessage(m_hWnd, SB_GETPARTS, 0L, 0L);
		}

		HICON GetPartIcon(int iPart)
		{
			assert(::IsWindow(m_hWnd));
			return (HICON)::SendMessage(m_hWnd, SB_GETICON, (WPARAM)iPart, 0L);
		}

		BOOL GetPartRect(int iPart, LPRECT prc)
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, SB_GETRECT, (WPARAM)iPart, (LPARAM)prc);
		}

		int GetPartTextLength(int iPart)
		{
			assert(::IsWindow(m_hWnd));
			return LOWORD(::SendMessage(m_hWnd, SB_GETTEXTLENGTH, iPart, 0L));
		}

		String GetPartText(int iPart)
		{
			assert(::IsWindow(m_hWnd));
			int iChars = LOWORD(::SendMessage(m_hWnd, SB_GETTEXTLENGTH, iPart, 0L));
			String text;
			text.resize(iChars);
			::SendMessage(m_hWnd, SB_GETTEXT, iPart, reinterpret_cast<LPARAM>(text.pointer()));
			return text;
		}

		BOOL IsSimple()
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, SB_ISSIMPLE, 0L, 0L);
		}

		BOOL SetPartText(int iPart, LPCTSTR szText, UINT Style = 0)
		// Available Styles: Combinations of ...
		//0					The text is drawn with a border to appear lower than the plane of the window.
		//SBT_NOBORDERS		The text is drawn without borders.
		//SBT_OWNERDRAW		The text is drawn by the parent window.
		//SBT_POPOUT		The text is drawn with a border to appear higher than the plane of the window.
		//SBT_RTLREADING	The text will be displayed in the opposite direction to the text in the parent window.
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, SB_SETTEXT, iPart | Style, (LPARAM)szText);
		}

		BOOL SetPartIcon(int iPart, HICON hIcon)
		{
			assert(::IsWindow(m_hWnd));
			return (BOOL)::SendMessage(m_hWnd, SB_SETICON, (WPARAM)iPart, (LPARAM) hIcon);
		}

		void SetSimple(BOOL fSimple /* = TRUE*/)
		{
			assert(::IsWindow(m_hWnd));
			::SendMessage(m_hWnd, SB_SIMPLE, (WPARAM)fSimple, 0L);
		}

	};

	class HStatusBar : public StatusBar<HWindow>
	{
	public:
		static HStatusBar *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id)
		{
			HWindow *pWnd = CreateEx(0, STATUSCLASSNAME, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HStatusBar *>(pWnd);
		}
	};

	template<class Super>
	class TabCtrl : public Super
	{
	public:
		BOOL GetItem(int index, TCITEM *item)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetItem(m_hWnd, index, item);
		}

		BOOL SetItem(int index, TCITEM *item)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetItem(m_hWnd, index, item);
		}

		int GetItemCount()
		// Retrieves the number of items in a tab control.
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetItemCount(m_hWnd);
		}

		int InsertItem(int index, const TCITEM *item)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_InsertItem(m_hWnd, index, item);
		}

		BOOL DeleteItem(int index)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_DeleteItem(m_hWnd, index);
		}

		BOOL DeleteAllItems()
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_DeleteAllItems(m_hWnd);
		}

		BOOL GetItemRect(int index, RECT *rect)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetItemRect(m_hWnd, index, rect);
		}

		int GetCurSel()
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetCurSel(m_hWnd);
		}

		int SetCurSel(int index)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetCurSel(m_hWnd, index);
		}

		int HitTest(TCHITTESTINFO *info)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_HitTest(m_hWnd, info);
		}

		BOOL SetItemExtra(UINT cb)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetItemExtra(m_hWnd, cb);
		}

		int AdjustRect(BOOL larger, RECT *rect)
		{
			assert(::IsWindow(m_hWnd));
			return TabCtrl_AdjustRect(m_hWnd, larger, rect);
		}

		DWORD SetItemSize(int x, int y)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetItemSize(m_hWnd, x, y);
		}

		void RemoveImage(int index)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_RemoveImage(m_hWnd, index);
		}

		void SetPadding(int cx, int cy)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			TabCtrl_SetPadding(m_hWnd, cx, cy);
		}

		int GetRowCount()
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetRowCount(m_hWnd);
		}

		HWindow *GetToolTips()
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetToolTips(m_hWnd);
		}

		void SetToolTips(HWND hwndTT)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetToolTips(m_hWnd, hwndTT);
		}

		int GetCurFocus()
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetCurFocus(m_hWnd);
		}

		int SetCurFocus(int index)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetCurFocus(m_hWnd, index);
		}

		int SetMinTabWidth(int x)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetMinTabWidth(m_hWnd, x);
		}

		void DeselectAll(BOOL excludeFocus)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_DeselectAll(m_hWnd, excludeFocus);
		}

		DWORD SetExtendedStyle(DWORD dw)
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_SetExtendedStyle(m_hWnd, dw);
		}

		DWORD GetExtendedStyle()
		{
			using ::SendMessage;
			assert(::IsWindow(m_hWnd));
			return TabCtrl_GetExtendedStyle(m_hWnd);
		}

	};

	class HTabCtrl : public TabCtrl<HWindow>
	{
	public:
		static HTabCtrl *Create(DWORD style, int x, int y, int cx, int cy, HWindow *parent, UINT id, DWORD xstyle = 0)
		{
			HWindow *pWnd = CreateEx(xstyle, WC_TABCONTROL, NULL, style, x, y, cx, cy, parent, id);
			return static_cast<HTabCtrl *>(pWnd);
		}
	};

	class ToolBarButton : public TBBUTTON
	{
	public:
		ToolBarButton(int id)
		{
			idCommand = id;
		}
		ToolBarButton(ToolBarButton *p)
		{
			for (iBitmap = 0 ; p < this ; ++p)
			{
				ZeroMemory(&p->idCommand + 1, sizeof(TBBUTTON) - RTL_SIZEOF_THROUGH_FIELD(TBBUTTON, idCommand));
				p->fsState = TBSTATE_ENABLED;
				if (p->idCommand)
				{
					p->fsStyle = TBSTYLE_BUTTON;
					p->iBitmap = iBitmap++;
				}
				else
				{
					p->fsStyle = TBSTYLE_SEP;
					p->iBitmap = 6;
				}
			}
		}
	};

	template<class Super>
	class SysString : public Super
	{
	public:
		UINT Len() const
		{
			return SysStringLen(const_cast<BSTR>(B));
		}
		UINT ByteLen() const
		{
			return SysStringByteLen(const_cast<BSTR>(B));
		}
		UINT InStr(LPCWSTR p) const
		{
			p = StrStrW(W, p);
			return p ? p + 1 - W : 0;
		}
		void Free()
		{
			SysFreeString(B);
		}
	};

	class HString : public SysString<Handle>
	{
	public:
		using SysString<Handle>::B;
		using SysString<Handle>::A;
		using SysString<Handle>::W;
		using SysString<Handle>::T;
		static HString *Uni(LPCWSTR W)
		{
			return reinterpret_cast<HString *>(SysAllocString(W));
		}
		static HString *Uni(LPCWSTR W, UINT lenW)
		{
			return reinterpret_cast<HString *>(SysAllocStringLen(W, lenW));
		}
		static HString *Oct(LPCSTR A)
		{
			return reinterpret_cast<HString *>(SysAllocStringByteLen(A, lstrlenA(A)));
		}
		static HString *Oct(LPCSTR A, UINT lenA)
		{
			return reinterpret_cast<HString *>(SysAllocStringByteLen(A, lenA));
		}
		// Uni(codepage) converts string using MultiByteToWideChar()
		HString *Uni(UINT codepage = CP_ACP) // passing 1200 makes this a NOP
		{
			HString *pStr = this;
			if (codepage != 1200) // 1200 means 'no conversion'
			{
				int a = SysStringByteLen(B);
				int w = MultiByteToWideChar(codepage, 0, A, a, 0, 0);
				pStr = reinterpret_cast<HString *>(SysAllocStringLen(0, w));
				MultiByteToWideChar(codepage, 0, A, a, pStr->W, w);
				Free();
			}
			return pStr;
		}
		// Oct(codepage) converts string using WideCharToMultiByte()
		HString *Oct(UINT codepage = CP_ACP) // passing 1200 makes this a NOP
		{
			HString *pStr = this;
			if (codepage != 1200) // 1200 means 'no conversion'
			{
				int w = SysStringLen(B);
				int a = WideCharToMultiByte(codepage, 0, W, w, 0, 0, 0, 0);
				pStr = reinterpret_cast<HString *>(SysAllocStringByteLen(0, a));
				WideCharToMultiByte(codepage, 0, W, w, pStr->A, a, 0, 0);
				Free();
			}
			return pStr;
		}
		HString *Trim(LPCWSTR exclude = L" \t\r\n")
		{
			HString *pStr = this;
			if (StrTrimW(W, exclude))
			{
				BSTR B = pStr->B;
				SysReAllocStringLen(&B, NULL, lstrlenW(W));
				pStr = reinterpret_cast<HString *>(B);
			}
			return pStr;
		}
		HString *Append(LPCWSTR src)
		{
			HString *pStr = this;
			BSTR B = pStr->B;
			UINT len = Len();
			if (SysReAllocStringLen(&B, NULL, len + lstrlenW(src)))
			{
				::StrCpyW(B + len, src);
			}
			else
			{
				::SysFreeString(B);
				B = NULL;
			}
			pStr = reinterpret_cast<HString *>(B);
			return pStr;
		}
		HString *ToLower()
		{
			CharLowerW(W);
			return this;
		}
		HString *ToUpper()
		{
			CharUpperW(W);
			return this;
		}
	};

	union UNotify
	{
		HWND hwndFrom;
		HWindow *pwndFrom;
		HTreeView *pTV;
		HListView *pLV;
		HTabCtrl *pTC;
		HStatusBar *pSB;
		HComboBox *pCB;
		NMHDR HDR;
		NMKEY KEY;
		NMCUSTOMDRAW CUSTOMDRAW;
		NMTVKEYDOWN TVKEYDOWN;
		NMTREEVIEW TREEVIEW;
		NMTVITEMCHANGE TVITEMCHANGE;
		NMTVDISPINFO TVDISPINFO;
		NMTVCUSTOMDRAW TVCUSTOMDRAW;
		NMLVCUSTOMDRAW LVCUSTOMDRAW;
		NMCOMBOBOXEX COMBOBOXEX;
	};

}
