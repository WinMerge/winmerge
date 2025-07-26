// WildcardDropList.cpp
// Copyright (c) datadiode
// SPDX-License-Identifier: WTFPL

#include "stdafx.h"
#if 0 // Change to 1 if in doubt whether stdafx.h includes them already
#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>
#include <algorithm>
#endif

#include "WildcardDropList.h"
#include "MergeDarkMode.h"

/**
 * @brief DropList window procedure.
 */
LRESULT WildcardDropList::LbWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DRAWITEM:
		if (wParam == IDCANCEL)
		{
			DRAWITEMSTRUCT *const pdis = reinterpret_cast<DRAWITEMSTRUCT *>(lParam);
			::DrawFrameControl(pdis->hDC, &pdis->rcItem, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT | DFCS_MONO);
			return 1;
		}
		break;
	case WM_LBUTTONDOWN:
		if (HWND hTc = GetDlgItem(hwnd, 100))
		{
			TCHITTESTINFO info;
			POINTSTOPOINT(info.pt, lParam);
			int i = TabCtrl_HitTest(hTc, &info);
			if (i != -1)
			{
				TCITEM item;
				item.mask = TCIF_STATE;
				item.dwStateMask = TCIS_HIGHLIGHTED;
				TabCtrl_GetItem(hTc, i, &item);
				item.dwState ^= TCIS_HIGHLIGHTED;
				TabCtrl_SetItem(hTc, i, &item);
				::EnableWindow(hTc, TRUE);
			}
			else if (HWND hwndHit = ::ChildWindowFromPoint(hwnd, info.pt))
			{
				if (hwndHit != hwnd && ::GetDlgCtrlID(hwndHit) == IDCANCEL)
				{
					if (HWND hCb = reinterpret_cast<HWND>(::GetWindowLongPtr(hTc, GWLP_USERDATA)))
					{
						::EnableWindow(hTc, FALSE);
						::SendMessage(hCb, CB_SHOWDROPDOWN, 0, 0);
					}
				}
			}
		}
		break;
	case WM_RBUTTONDOWN:
		if (HWND hTc = ::GetDlgItem(hwnd, 100))
		{
			if (HWND hCb = reinterpret_cast<HWND>(::GetWindowLongPtr(hTc, GWLP_USERDATA)))
			{
				::SendMessage(hCb, CB_SHOWDROPDOWN, 0, 0);
			}
		}
		break;
	case WM_HOTKEY:
		if (wParam == IDCANCEL)
		{
			if (HWND hTc = ::GetDlgItem(hwnd, 100))
			{
				if (HWND hCb = reinterpret_cast<HWND>(::GetWindowLongPtr(hTc, GWLP_USERDATA)))
				{
					::EnableWindow(hTc, FALSE);
					::SendMessage(hCb, CB_SHOWDROPDOWN, 0, 0);
				}
			}
		}
		break;
	}
	WNDPROC pfnSuper = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
	return ::CallWindowProc(pfnSuper, hwnd, message, wParam, lParam);
}

/**
 * @brief Handles the CBN_DROPDOWN notification.
 * @param [in] hCb Handle to ComboBox control.
 * @param [in] columns Number of columns to fit in one line.
 * @param [in] fixedPatterns Semicolon delimited list of wildcard patterns.
 * @param [in] allowUserAddedPatterns Whether to allow user-added patterns
 */
void WildcardDropList::OnDropDown(HWND hCb, int columns, LPCTSTR fixedPatterns, bool allowUserAddedPatterns)
{
	COMBOBOXINFO info{ sizeof info };
	if (!::GetComboBoxInfo(hCb, &info))
		return;
	RECT rc, rcCombo;
	::GetClientRect(info.hwndList, &rc);
	::GetWindowRect(hCb, &rcCombo);
	int const cxCross = ::GetSystemMetrics(SM_CXVSCROLL);
	::CreateWindow(WC_BUTTON, NULL,
		WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		rc.right - cxCross, 0, cxCross, cxCross,
		info.hwndList, reinterpret_cast<HMENU>(IDCANCEL), NULL, NULL);
	HWND const hTc = ::CreateWindow(WC_TABCONTROL, NULL,
		WS_CHILD | WS_VISIBLE | WS_DISABLED | TCS_BUTTONS |
		TCS_FIXEDWIDTH | TCS_FORCELABELLEFT | TCS_MULTILINE,
		0, 0, rc.right, 10000,
		info.hwndList, reinterpret_cast<HMENU>(100), NULL, NULL);
	::SetWindowLongPtr(hTc, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(hCb));
	::SendMessage(hTc, WM_SETFONT, ::SendMessage(hCb, WM_GETFONT, 0, 0), 0);
	TabCtrl_SetItemSize(hTc, (rc.right - cxCross) / columns - 3, (rcCombo.bottom - rcCombo.top));
	int const len = ::GetWindowTextLength(hCb) + 1;
	TCHAR *const patterns = static_cast<TCHAR *>(_alloca(len * sizeof(TCHAR)));
	::GetWindowText(hCb, patterns, len);
	int i = 0;
	LPCTSTR pch = fixedPatterns;
	while (size_t const cch = _tcscspn(pch += _tcsspn(pch, _T("; ")), _T("; ")))
	{
		TCHAR text[20];
		*std::copy<>(pch, pch + std::min<>(cch, _countof(text) - 1), text) = _T('\0');
		TCITEM item;
		item.dwStateMask = TCIS_HIGHLIGHTED;
		item.dwState = (patterns[0] && PathMatchSpec(text, patterns)) ? TCIS_HIGHLIGHTED : 0;
		item.pszText = text;
		item.mask = TCIF_TEXT;
		TabCtrl_InsertItem(hTc, i, &item);
		item.mask = TCIF_STATE;
		TabCtrl_SetItem(hTc, i, &item);
		++i;
		pch += cch;
	}
	if (allowUserAddedPatterns)
	{
		pch = patterns;
		while (size_t const cch = _tcscspn(pch += _tcsspn(pch, _T("; ")), _T("; ")))
		{
			TCHAR text[20];
			*std::copy<>(pch, pch + std::min<>(cch, _countof(text) - 1), text) = _T('\0');
			if (!fixedPatterns[0] || !PathMatchSpec(text, fixedPatterns))
			{
				TCITEM item;
				item.dwStateMask = TCIS_HIGHLIGHTED;
				item.dwState = TCIS_HIGHLIGHTED;
				item.pszText = text;
				item.mask = TCIF_TEXT;
				TabCtrl_InsertItem(hTc, i, &item);
				item.mask = TCIF_STATE;
				TabCtrl_SetItem(hTc, i, &item);
				++i;
			}
			pch += cch;
		}
	}
	TabCtrl_SetCurSel(hTc, -1);
	TabCtrl_AdjustRect(hTc, FALSE, &rc);
	rc.right = static_cast<int>(::SendMessage(hCb, CB_GETDROPPEDWIDTH, 0, 0));
	::SetWindowPos(info.hwndList, NULL, 0, 0, rc.right, rc.top, SWP_NOMOVE | SWP_NOZORDER);
	::RegisterHotKey(info.hwndList, IDCANCEL, 0, VK_ESCAPE);
	LONG_PTR pfnSuper = ::SetWindowLongPtr(info.hwndList, GWLP_WNDPROC, (LONG_PTR)LbWndProc);
	::SetWindowLongPtr(info.hwndList, GWLP_USERDATA, pfnSuper);
#if defined(USE_DARKMODELIB)
	DarkMode::setTabCtrlSubclass(hTc);
#endif
}

/**
 * @brief Handles the CBN_CLOSEUP notification.
 * @param [in] hCb Handle to ComboBox control.
 */
bool WildcardDropList::OnCloseUp(HWND hCb)
{
	COMBOBOXINFO info{ sizeof info };
	if (!::GetComboBoxInfo(hCb, &info))
		return false;
	::UnregisterHotKey(info.hwndList, IDCANCEL);
	bool ret = false;
	if (HWND const hTc = ::GetDlgItem(info.hwndList, 100))
	{
		if (::IsWindowEnabled(hTc))
		{
			TCHAR text[20];
			int const n = TabCtrl_GetItemCount(hTc);
			TCHAR *const patterns = static_cast<TCHAR *>(
				_alloca(n * _countof(text) * sizeof(TCHAR)));
			TCHAR *pch = patterns;
			for (int i = 0; i < n; ++i)
			{
				TCITEM item;
				item.pszText = text;
				item.cchTextMax = _countof(text);
				item.mask = TCIF_TEXT | TCIF_STATE;
				item.dwStateMask = TCIS_HIGHLIGHTED;
				TabCtrl_GetItem(hTc, i, &item);
				if (item.dwState & TCIS_HIGHLIGHTED)
				{
					if (pch > patterns)
						*pch++ = _T(';');
					while (TCHAR ch = *item.pszText++)
						*pch++ = ch;
				}
			}
			*pch = _T('\0');
			::SetWindowText(hCb, patterns);
			::SendMessage(hCb, CB_SETEDITSEL, 0, MAKELPARAM(0, -1));
			ret = true;
		}
		::DestroyWindow(hTc);
	}
	LONG_PTR pfnSuper = ::SetWindowLongPtr(info.hwndList, GWLP_USERDATA, 0);
	::SetWindowLongPtr(info.hwndList, GWLP_WNDPROC, pfnSuper);
	return ret;
}

LRESULT WildcardDropList::LvWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pfnSuper = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (message)
	{
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
			TCHAR text[4096];
			LONG_PTR data;
		case CBN_CLOSEUP:
			OnCloseUp(reinterpret_cast<HWND>(lParam));
			::GetWindowText(reinterpret_cast<HWND>(lParam), text, _countof(text));
			data = ::GetWindowLongPtr(reinterpret_cast<HWND>(lParam), GWLP_USERDATA);
			ListView_SetItemText(hwnd, SHORT LOWORD(data), SHORT HIWORD(data), text);
			::DestroyWindow(reinterpret_cast<HWND>(lParam));
			::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)pfnSuper);
			::SetFocus(hwnd);
			break;
		case CBN_SELENDOK:
			::GetWindowText(reinterpret_cast<HWND>(lParam), text, _countof(text));
			data = ::GetWindowLongPtr(reinterpret_cast<HWND>(lParam), GWLP_USERDATA);
			ListView_SetItemText(hwnd, SHORT LOWORD(data), SHORT HIWORD(data), text);
			break;
		}
		return 0;
	}
	return ::CallWindowProc(pfnSuper, hwnd, message, wParam, lParam);
}

void WildcardDropList::OnItemActivate(HWND hLv, int iItem, int iSubItem, int columns, LPCTSTR fixedPatterns, bool allowUserAddedPatterns, int limitTextSize)
{
	RECT rc;
	ListView_EnsureVisible(hLv, iItem, FALSE);
	ListView_GetSubItemRect(hLv, iItem, iSubItem, LVIR_BOUNDS, &rc);
	TCHAR text[4096];
	ListView_GetItemText(hLv, iItem, iSubItem, text, _countof(text));
	HWND hCb = ::CreateWindow(WC_COMBOBOX, NULL, WS_CHILD | WS_VISIBLE |
		WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT,
		rc.left, rc.top - 1, rc.right - rc.left, 0,
		hLv, reinterpret_cast<HMENU>(1), NULL, NULL);
	::SetWindowLongPtr(hCb, GWLP_USERDATA, MAKELPARAM(iItem, iSubItem));
	::SendMessage(hCb, WM_SETFONT, ::SendMessage(hLv, WM_GETFONT, 0, 0), 0);
	::SetFocus(hCb);
	::SetWindowText(hCb, text);

	size_t len = _tcslen(text);
	LPARAM lp = (len << 16) | len; 
	::SendMessage(hCb, CB_SETEDITSEL, 0, lp);

	if (limitTextSize > 0)
		::SendMessage(hCb, CB_LIMITTEXT, limitTextSize, 0);

	LONG_PTR pfnSuper = ::SetWindowLongPtr(hLv, GWLP_WNDPROC, (LONG_PTR)LvWndProc);
	::SetWindowLongPtr(hLv, GWLP_USERDATA, pfnSuper);
	OnDropDown(hCb, columns, fixedPatterns, allowUserAddedPatterns);
	::SendMessage(hCb, CB_SHOWDROPDOWN, TRUE, 0);
}
