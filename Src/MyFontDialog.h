/**
 * @file MyFontDialog.h
 *
 * @brief Declaration of a custom font dialog that supports dark mode.
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>

class CMyFontDialog : public CFontDialog
{
public:
	CMyFontDialog(LPLOGFONT lplfInitial = NULL, DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS, CDC* pdcPrinter = NULL, CWnd* pParentWnd = NULL);
	CMyFontDialog(const CHARFORMAT& charformat, DWORD dwFlags = CF_SCREENFONTS, CDC* pdcPrinter = NULL, CWnd* pParentWnd = NULL);
};