/**
 * @file MyFontDialog.cpp
 *
 * @brief Implementation of a custom font dialog that supports dark mode.
 */
#include "StdAfx.h"
#include "MyFontDialog.h"
#include "MergeDarkMode.h"

static void init(CFontDialog& dlg)
{
#if defined(USE_DARKMODELIB)
	if (DarkMode::isEnabled())
	{
		dlg.m_cf.Flags |= CF_ENABLEHOOK | CF_ENABLETEMPLATE;
		dlg.m_cf.lpfnHook = [](HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) -> UINT_PTR {
			if (msg == WM_INITDIALOG) LangTranslateDialog(hdlg);
			return DarkMode::HookDlgProc(hdlg, msg, wParam, lParam);
		};
		dlg.m_cf.hInstance = GetModuleHandle(nullptr);
		dlg.m_cf.lpTemplateName = MAKEINTRESOURCE(IDD_DARK_FONT_DIALOG);
	}
#endif
}

CMyFontDialog::CMyFontDialog(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd)
	: CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
{
	init(*this);
}

CMyFontDialog::CMyFontDialog(const CHARFORMAT& charformat, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd)
	: CFontDialog(charformat, dwFlags, pdcPrinter, pParentWnd)
{
	init(*this);
}